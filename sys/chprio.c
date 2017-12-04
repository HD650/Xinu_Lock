/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

extern int g_lock_table[NPROC][NLOCKS];

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
        //if the prio of a process changed, this may cause a recusive change of
        //priority
	int l=0;
        for(;l<NLOCKS;l++)
        {
          if(g_lock_table[pid][l]==WAIT)
            if(g_locks[l].lmaxprio<newprio)
            {
              g_locks[l].lmaxprio=newprio;
              int p=0;
              for(;p<NPROC;p++)
              {
                if(g_lock_table[p][l]==HOLD)
                  chprio(p,g_locks[l].lmaxprio);
              }
            }
        }
        restore(ps);
	return(newprio);
}
