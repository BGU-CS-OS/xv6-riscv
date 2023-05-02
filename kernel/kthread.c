#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

extern struct proc proc[NPROC];

extern void forkret(void);


void kthreadinit(struct proc *p)
{
  initlock(&p->threadlock, "threadslock");
  for (struct kthread *kt = p->kthread; kt < &p->kthread[NKT]; kt++)
  {
    initlock(&kt->lock, "kthread");
    acquire(&kt->lock);
    kt->state = K_UNUSED;
    kt->parent = proc;
    release(&kt->lock);

    // WARNING: Don't change this line!
    // get the pointer to the kernel stack of the kthread
    kt->kstack = KSTACK((int)((p - proc) * NKT + (kt - p->kthread)));
  }
}

struct kthread *mykthread()
{
  push_off();
  struct cpu *c = mycpu();
  struct kthread *k = c->kthread;
  pop_off();
  return k;
}

struct trapframe *get_kthread_trapframe(struct proc *p, struct kthread *kt)
{
  return p->base_trapframes + ((int)(kt - p->kthread));
}

// TODO: delte this after you are done with task 2.2
// void allocproc_help_function(struct proc *p) {
  // p->kthread->trapframe = get_kthread_trapframe(p, p->kthread);

  // p->context.sp = p->kthread->kstack + PGSIZE;
// }
int alloc_kpid(struct proc *p){
  int ktid;
	acquire(&p->threadlock);
	ktid = p->counter;
  p->counter++;
	release(&p->threadlock);
	return ktid;
}

struct kthread* alloc_kthread(struct proc* p){

  struct kthread *k;
	for (k = p->kthread; k < &p->kthread[NKT]; k++){

    acquire(&k->lock);
    if(k->state == K_UNUSED) {
      goto found;
    } else {
      release(&k->lock);
    }
  }
  return 0;
found:
  k->Kpid = alloc_kpid(p);
	k->state = K_USED;
	k->trapframe = get_kthread_trapframe(p, k);
	memset(&k->context, 0, sizeof(k->context));
	k->context.ra = (uint64)forkret;
	k->context.sp = k->kstack + PGSIZE;
	return k;
}
//todo
void free_kthread(struct kthread* k){
  k->Kpid = 0;
  k->chan = 0;
  k->killed = 0;
  k->xstate = 0;
  k->trapframe = 0;
  k->state = K_UNUSED;
}


