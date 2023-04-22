#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];

extern void forkret(void);


void
kthreadinit(struct proc *p)
{
  initlock(&p->counter_lock, "tid_counter");
  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->t_lock, "thread");
      kt->t_state = T_UNUSED;
    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->t_kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
  }
}



struct kthread*
mykthread(void)
{
  push_off();
  struct cpu *c = mycpu();
  struct kthread *kt = c->thread;
  pop_off();
  return kt;
}


int
alloc_tid(struct proc* p)
{
  int tid;
  
  acquire(&p->counter_lock);
  tid = p->tid_counter;
  p->tid_counter = p->tid_counter + 1;
  release(&p->counter_lock);

  return tid;
}

static struct kthread*
allockthread(struct proc* p)
{
  struct kthread *kt=0;
  for (kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    acquire(&kt->t_lock);
    if(kt->t_state==T_UNUSED)  {
      goto found;
    } 
    else {
      release(&kt->t_lock);
    }
  }
  return 0;

  found:
  kt->tid = alloc_tid();
  kt->t_state = T_USED;

  // Allocate a trapframe page.
  if (!(kt->t_trapframe = get_kthread_trapframe(p, kt))){
    freekthread(kt);
    release(&kt->t_lock);
    return 0;
  }


  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&kt->context, 0, sizeof(kt->context));
  kt->context.ra = (uint64)forkret;
  kt->context.sp = kt->t_kstack + PGSIZE;

  return kt;
}
    


static void
freekthread(struct kthread *kt)
{
  if(kt->t_trapframe)
    kfree((void*)kt->t_trapframe);
  kt->t_trapframe = 0;
  kt->tid = 0;
  kt->t_chan = 0;
  kt->t_killed = 0;
  kt->t_xstate = 0;
  kt->t_state = T_UNUSED;
  kt->pcb = 0;
}



struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

