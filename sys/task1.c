#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>

#define assert(x,error) if(!(x)){ \
            kprintf(error);\
            return;\
            }

void lock_worker(char* name, int locknum)
{
  kprintf("%s running...\n",name);
  lock(locknum, WRITE, proctab[currpid].pprio);
  kprintf("%s got lock...\n",name);
  int i,iii;
  for(i=0;i<10000000;i++)
      iii++;
  releaseall(1,locknum);
  kprintf("%s released lock...\n",name);
}

void worker(char* name)
{
  kprintf("%s running...\n",name);
  int i,iii;
  for(i=0;i<10000000;i++)
      iii++;
  kprintf("%s done...\n",name);
}

void sem_worker(char* name, int semnum)
{
  kprintf("%s running...\n",name);
  wait(semnum);
  kprintf("%s got lock...\n",name);
  int i,iii;
  for(i=0;i<10000000;i++)
    iii++;
  signal(semnum);
  kprintf("%s released lock...\n",name);
}


int main()
{
  kprintf("---lock with priority inheritance---\n");
  int lock=lcreate();
  int pid1=create(lock_worker,2000,20,"lock_worker_20",2,"lock_worker_20",lock);
  int pid2=create(lock_worker,2000,30,"lock_worker_30",2,"lock_worker_30",lock);
  int pid3=create(worker,2000,25,"worker_25",2,"worker_25",lock);
  resume(pid1);
  resume(pid2);
  resume(pid3);
  sleep(5);
  
  kprintf("---sem without priority inheritance---\n");
  int sem=screate(1);
  pid1=create(sem_worker,2000,20,"sem_worker_20",2,"sem_worker_20",sem);
  pid2=create(sem_worker,2000,30,"sem_worker_30",2,"sem_worker_30",sem);
  pid3=create(worker,2000,25,"worker_25",2,"worker_25",sem);  
  resume(pid1);
  resume(pid2);
  resume(pid3);
  sleep(5);
}
