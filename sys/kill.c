/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

extern int g_lock_table[NPROC][NLOCKS];

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
        //if a process is killed, the priority of the lock may change
        int l=0;
        for(l=0;l<NLOCKS;l++)
        {
          //change only the lock this process wait
          if(g_lock_table[pid][l]==WAIT)
          {
            //after the if statement, we should clear this process in table
            g_lock_table[pid][l]=0;
            if(g_locks[l].lmaxprio==proctab[pid].pprio)
            {
              int p=0;
              int max=MININT;
              for(;p<NPROC;p++)
                if(g_lock_table[p][l]==WAIT)
                  if(proctab[p].pprio>max||proctab[p].pprio<g_locks[l].lmaxprio)
                    max=proctab[p].pprio;
              g_locks[l].lmaxprio=max;
              for(p=0;p<NPROC;p++)
                if(g_lock_table[p][l]==HOLD)
                  chprio(p,max);
            }
          }
        }
	restore(ps);
	return(OK);
}
