#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <q.h>

extern int g_lock_table[NPROC][NLOCKS];

int lock(int ldes, int type, int priority)
{
  STATWORD ps;
  disable(ps);
  int index=ldes%NLOCKS;
  //the index is invaild or the lock is not created yet
  if(!lock_check(index)||g_locks[index].lstate==LFREE)
  {
    restore(ps);
    return SYSERR;
  }
  //test if this lock is already free and reused, if reused, return SYSERR
  if(g_locks[index].lid!=ldes)
  {
    restore(ps);
    return SYSERR;
  }
  //if there is no holder in this lock, this lock is held
  if(g_locks[index].lnumh==0)
  {
    g_locks[index].ltype=type;
    g_locks[index].lholder=currpid;
    g_locks[index].lnumh++;
    if(type==READ)
      g_locks[index].lnumr++;
    else if(type==WRITE)
      g_locks[index].lnumw++;
    //now this lock is held by currpro
    g_lock_table[currpid][index]=HOLD;
    //if some of the waiting processes has higher priority than this one
    if(proctab[currpid].pprio<g_locks[index].lmaxprio)
    {
      chprio(currpid,g_locks[index].lmaxprio);
    }
    restore(ps);
    return OK;
  }
  //if we want a read lock, it depends
  if(type==READ)
  {
    //if this lock is held by a writing task, read task should wait
    if(g_locks[index].ltype==WRITE)
    {
      //we don't use PRWAIT since that state will be waked by scheder
      //we want wake these processes by ourself
      proctab[currpid].pstate=PLWAIT;
      proctab[currpid].pwaitret=OK;
      //insert this process into the wait queue
      insert(currpid,g_locks[index].lqueuehead,priority);
      //save the job type, so when wake this process, we can change the 
      //type of the lock
      q[currpid].qtype=READ;
      g_lock_table[currpid][index]=WAIT;
      //if this process is waiting, update the max priority in lock
      if(proctab[currpid].pprio>g_locks[index].lmaxprio)
      {
        g_locks[index].lmaxprio=proctab[currpid].pprio;
        //if the max priority of this lock is changed, update all holder
        int p=0;
        for(;p<NPROC;p++)
        {
          if(g_lock_table[p][index]==HOLD)
            chprio(p,g_locks[index].lmaxprio);
        }
      }
      //give out the cpu
      resched();
      restore(ps);
      //return the return value, may be OK ot DELETED
      return proctab[currpid].pwaitret;
    }
    //if this lock is held by a reading task
    else if(g_locks[index].ltype==READ)
    {
      //if the priority is bigger than the max writing task in queue
      //this process can hold this lock
      if(g_locks[index].lmaxw<=priority)
      {
        g_locks[index].lholder=currpid;
        g_locks[index].lnumh++;
        g_locks[index].lnumr++;
        g_lock_table[currpid][index]=HOLD;
        if(proctab[currpid].pprio<g_locks[index].lmaxprio)
        {
          chprio(currpid,g_locks[index].lmaxprio);
        }
        restore(ps);
        return OK;
      }
      else
      {
        proctab[currpid].pstate=PLWAIT;
        proctab[currpid].pwaitret=OK;
        //insert this process into the wait queue
        insert(currpid,g_locks[index].lqueuehead,priority);
        //save the job type, so when wake this process, we can change the 
        //type of the lock
        q[currpid].qtype=type;
        g_lock_table[currpid][index]=WAIT;
        if(proctab[currpid].pprio>g_locks[index].lmaxprio)
        {
          g_locks[index].lmaxprio=proctab[currpid].pprio;
          int p=0;
          for(;p<NPROC;p++)
          {
            if(g_lock_table[p][index]==HOLD)
              chprio(p,g_locks[index].lmaxprio);
          }
        }
        resched();
        restore(ps);
        return proctab[currpid].pwaitret; 
      }
    }
  }
  //if we want a write lock, we can't get it since writing is mutex
  else if(type==WRITE)
  {
      //we don't use PRWAIT since that state will be waked by scheder
      //we want wake these processes by ourself
      proctab[currpid].pstate=PLWAIT;
      proctab[currpid].pwaitret=OK;
      //insert this process into the wait queue
      insert(currpid,g_locks[index].lqueuehead,priority);
      //save the job type, so when wake this process, we can change the 
      //type of the lock
      q[currpid].qtype=type;
      if(priority>g_locks[index].lmaxw)
        g_locks[index].lmaxw=priority;
      g_lock_table[currpid][index]=WAIT;
      if(proctab[currpid].pprio>g_locks[index].lmaxprio)
      {
        g_locks[index].lmaxprio=proctab[currpid].pprio;
        //if the max priority of this lock is changed, update all holder
        int p=0;
        for(;p<NPROC;p++)
        {
          if(g_lock_table[p][index]==HOLD)
            chprio(p,g_locks[index].lmaxprio);
        }
      }
      //give out the cpu
      resched();
      restore(ps);
      //return the return value, may be OK ot DELETED
      return proctab[currpid].pwaitret; 
  }
}
