#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];

extern void forkret(void);

void kthread_init(struct proc *p)
{

  initlock(&p->threadslock, "threadslock");

  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->lock, "klock");
    kt->state = KT_UNUSED;
    kt->proc = p;
    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
  }
}


struct kthread *mykthread()
{
  push_off();
  struct cpu *c = mycpu();
  struct kthread *t = c->kt;
  pop_off();
  return t;
}


struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}


int alloc_tid(struct proc* p)
{
  acquire(&p->threadslock);
  int ktid;
  ktid = p->ktcounter;
  p->ktcounter++;
  release(&p->threadslock);
  return ktid;
}

/**
 * allocates a thread
 * @post: returned thread's lock is acquired
*/
struct kthread* alloc_thread(struct proc* p)
{
	struct kthread *kt;
	for (kt = p->kthread; kt < &p->kthread[NKT]; kt++)
	{
		acquire(&kt->lock);
		if (kt->state != KT_UNUSED)
			release(&kt->lock);
		else
		{
			kt->ktid = alloc_tid(p);
			kt->state = KT_USED;
			kt->trapframe = get_kthread_trapframe(p, kt);
			memset(&kt->context, 0, sizeof(kt->context));
			kt->context.ra = (uint64)forkret;
			kt->context.sp = kt->kstack + PGSIZE;
			return kt;
		}
	}
	return 0;
}

/**
 * resets thread fields
 * @pre: kt lock must be held
 * @post: kt lock remains held
*/
void
free_thread(struct kthread* kt)
{
  kt->ktid = 0;
  kt->chan = 0;
  kt->killed = 0;
  kt->xstate = 0;
  kt->trapframe = 0;
  kt->state = KT_UNUSED;
}
