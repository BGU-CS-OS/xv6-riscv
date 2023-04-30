#include "uthread.h"
#include "user.h"

#define MAX_UTHREADS 100
static struct uthread uthreads[MAX_UTHREADS];
struct uthread  *running_thread = NULL;
static int num_thread=0;//tracking the number of threads
static bool isInit = false;

int uthread_create(void (*start_func)(), enum sched_priority priority) {
    int i;

    // Find a free entry in the uthreads table
    for (i = 0; i < MAX_UTHREADS; i++) {
        if (uthreads[i].state == FREE) {
            break;
        }
    }

    if (i == MAX_UTHREADS) {
        // No free entry was found
        return -1;
    }
    // Allocate the stack for the new thread
    memset(&uthreads[i].context, 0, sizeof(uthreads[i].context));
    uthreads[i].context.ra = (uint64) start_func;  // ra register
    uthreads[i].context.sp = (uint64)uthreads[i].ustack;               // return address
    uthreads[i].state = RUNNABLE;
    uthreads[i].priority = priority;

    // Set the context for the new thread
    
    // Update the current thread and return success

    num_thread++;
    return 0;
}

void 
uthread_yield()
{  
    running_thread->state = RUNNABLE;
    struct uthread* next = get_next_thread();
    struct uthread* prev = running_thread;
    running_thread = next;
    uswtch(&prev->context, &next->context);
}
void 
uthread_exit()
{   

    running_thread->state = FREE;
    running_thread->priority = LOW;
    struct uthread* t;	
	for (t = uthreads ; t->state != FREE; t++);
    if (t==uthreads+MAX_UTHREADS)
        exit(0);
    struct uthread* next = get_next_thread();
    struct uthread* prev = running_thread;
    running_thread = next;
    uswtch(&prev->context, &next->context);
}

int uthread_start_all()
{
    if (isInit) {
        return -1; 
    }
    isInit = true;
    
    struct uthread* first_thread_to_run = get_next_thread();

    struct context c;
    memset(&c, 0, sizeof(c));

    if (first_thread_to_run) {
        running_thread = first_thread_to_run;
        first_thread_to_run->state = RUNNING;
        uswtch(&c, &first_thread_to_run->context);
    } 
    
    return 0;
}

enum sched_priority uthread_set_priority(enum sched_priority priority)
{
    enum sched_priority prev= running_thread->priority;
    running_thread->priority=priority;
    return prev;
}
enum sched_priority 
uthread_get_priority()
{   
    return running_thread->priority;
}

struct uthread *uthread_self()
{
    return running_thread;
}
