#ifndef LOCK_H
#define LOCK_H

#define NLOCKS 50
#define READ 0
#define WRITE 1

#define LFREE 'f'
#define LUSED 'u'

struct lock_t
{
  char lstate;
  int lqueuehead;
  int lqueuetail;
  int lholder;
  int lmaxprio;
  int ltype;
}

extern struct lock_t g_locks[];
extern int g_lock_id;

#define lock_check(des) (des<NLOCKS || des>=0)

#endif
