#include <lock.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

int releaseall(int numlocks, int ldes)
{
  STATWORD ps;
  disable(ps);
  unsigned long *saddr;
  //convert the input parameter to a array
  unsigned long *args=(unsigned long*)&ldes;
  int i=0;
  int locknum;
  int is_error=0;
  for(;i<numlocks;i++)
  {
    //get the lock num
    locknum=(int)*(args+i);
    //check if the lock vaild, if not, can't be release
    if(!lock_check(locknum)||g_locks[locknum].lstate==LFREE)
    {
      is_error=1;
      continue;
    }
    //if nobody hold this lock, this lock can't be release
    if(g_locks[locknum].lnumh==0)
    {
      is_error=1;
      continue;
    }
    //the num of writer using this lock can only be 0 or 1
    if(g_locks[locknum].lnumw==1)
    {
      //release the writer
      g_locks[locknum].lnumw--;
      g_locks[locknum].lnumh--;
    }
    //if it's not held by a writer, it's held by readers
    else
    {
      //release a reader
      g_locks[locknum].lnumr--;
      g_locks[locknum].lnumh--;
    }
    //if this lock is free now, wake up a process in the wait queue
    if(g_locks[locknum].lnumh==0)
    {
      int last_proc=q[g_locks[locknum].lqueuetail].qprev;
      //if the wait queue is empty, do nothing
      if(last_proc==g_locks[locknum].lqueuehead)
        continue;
      else
      {
        //wake up a reader is different from a writer
        if(q[last_proc].qtype==READ)
        {
         //dequeue this process from wait queue and ready it
          dequeue(last_proc);
          ready(last_proc,RESCHNO);
          g_locks[locknum].lnumr++;
          g_locks[locknum].lnumh++;
          int prev_proc=q[last_proc].qprev;
          int temp;
          //dequeue and ready all reader in the wait queue has bigger priority than
          //the biggest writer in the queue
          while(prev_proc!=g_locks[locknum].lqueuehead&&prev_proc!=-1)
          {
            if(q[prev_proc].qtype==READ&&q[prev_proc].qkey>g_locks[locknum].lmaxw)
            {
              temp=prev_proc;
              prev_proc=q[prev_proc].qprev;
              dequeue(temp);
              ready(temp,RESCHNO);
            }
            prev_proc=q[prev_proc].qprev;
          } 
        }
        //just wake up this write
        else
        {
          dequeue(last_proc);
          ready(last_proc,RESCHNO);
          g_locks[locknum].lnumw++;
          g_locks[locknum].lnumh++;
        }
      }
    }
  }
  restore(ps);
  return OK;
}
