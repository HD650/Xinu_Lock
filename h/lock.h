#ifndef LOCK_H
#define LOCK_H

#define NLOCKS 50
#define READ 0
#define WRITE 1

#define HOLD 3
#define WAIT 4

#define LFREE 'f'
#define LUSED 'u'

struct lock_t
{
  char lstate;
  int lqueuehead;
  int lqueuetail;
  int lid;
  int lholder;
  int lmaxprio;
  int ltype;
  int lnumr; //num of reader in this lock
  int lnumw; //num of writer in this lock
  int lnumh; //num of holder in this lock, if WRITE, it canonly be 0 or 1
  int lmaxw; //max priority of write task in queue
};

extern struct lock_t g_locks[];
extern int g_lock_id;

#define lock_check(des) (des<NLOCKS || des>=0)

#endif
