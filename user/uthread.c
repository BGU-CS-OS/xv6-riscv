#include "uthread.h"




struct uthread uthreads[MAX_UTHREADS]={0};
struct uthread* current;


int 
uthread_create(void (*start_func)(), enum sched_priority priority)
{
  struct uthread* t;

  for(t = uthreads; t < &uthreads[MAX_UTHREADS]; t++) {
    if(t->state == FREE) {
      goto found;
    }
  }
  return -1;
  
  found:
    t->context.ra = start_func;
    t->context.sp = t->ustack;
    t->context.sp++;
    t->priority=priority;
    t->state=RUNNABLE;
    return 0;
}


int
sched_priority_num(enum sched_priority priority)
{
    switch (priority)
    {
    case LOW:
        return 0;
        break;
    case MEDIUM:
        return 1;
        break;
    case HIGH:
        return 2;
        break;
    default:
        return -1;
        break;
    }
}


void 
pick_thread_to_run(struct uthread* thread_to_run)
{
    struct uthread* t=0;
    int max_priority = 0;
    // enum sched_priority { LOW, MEDIUM, HIGH };
    for(t = uthreads; t < &uthreads[MAX_UTHREADS]; t++) {
        int pr = sched_priority_num(t->priority);
        if(pr>max_priority){
            max_priority=pr;
        }
    }
    t = current;
    if (++t >= &uthreads[MAX_UTHREADS]) {
        t = uthreads;
    }

    for (int i = 0; i < MAX_UTHREADS; i++){
        if (t >= &uthreads[MAX_UTHREADS]) {
            t = uthreads;
        } 
        int pr = sched_priority_num(t->priority);
        if(pr == max_priority)   {
            thread_to_run = t;
            break;
        } 
        t++;   
    } 
}


void 
uthread_yield()
{  
    current->state = RUNNABLE;
    struct uthread* thread_to_run;
    pick_thread_to_run(thread_to_run);
    uswtch(&current->context, &thread_to_run->context);
}


int is_all_threads_free(void)
{
struct uthread* t;
for(t = uthreads; t < &uthreads[MAX_UTHREADS]; t++) {
    if (t->state != FREE){
        return 0;
    }
}
return 1;
}

void 
uthread_exit()
{
    current->state = FREE;
    if (is_all_threads_free()){
        exit(0);
    }
    struct uthread* thread_to_run;
    pick_thread_to_run(thread_to_run);
    uswtch(&current->context, &thread_to_run->context);
}



enum sched_priority 
uthread_set_priority(enum sched_priority priority)
{   
    enum sched_priority prev = current->priority;
    current->priority = priority;
    return prev;
}


enum sched_priority 
uthread_get_priority()
{
    return current->priority;
}


int 
uthread_start_all()
{
    if (uthread_start_flag) {
        return -1; // already started
    }
    uthread_start_flag = 1; // mark as started

    // Find the highest priority thread to run
    int max_priority = -1;
    struct uthread *next_thread = 0;
    for (int i = 0; i < NTHREAD; i++) {
        if (thread_table[i].state == T_RUNNABLE && thread_table[i].priority > max_priority) {
            max_priority = thread_table[i].priority;
            next_thread = &thread_table[i];
        }
    }

    if (next_thread) {
        // Switch to the selected thread
        uthread_switch(&main_thread, next_thread);
    }

    // We should never get here
    panic("uthread_start_all returned");






    /*current->state = RUNNABLE;
    struct uthread* thread_to_run;
    pick_thread_to_run(thread_to_run);
    uswtch(&current->context, &thread_to_run->context);
    */
}
struct uthread* 
uthread_self(){ 
    return current;
}
