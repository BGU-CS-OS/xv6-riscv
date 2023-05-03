#include "kernel/types.h"
#include "user/user.h"
#include "uthread.h"

static struct uthread threads[MAX_UTHREADS];

static struct uthread *pCurrThread = 0;

//Returns the index in the array where the current running thread is located.
int get_pCurrThread_index(){
    struct uthread *t;
    int curr_thread_index = -1;
    for(t = threads; t < &threads[MAX_UTHREADS]; t++) {
        curr_thread_index += 1;
        if(t == pCurrThread){
            break;
        }      
    }
    return curr_thread_index; 
}

//returns the next runnable thread to run based on the priority scheduling policy, or 0 if there are no runnable threads.
struct uthread* get_next_runnable_thread() {
    struct uthread *th;
    struct uthread *maxPrThread = 0;
    enum sched_priority max_priority = LOW;
    int first = 1;

    if(pCurrThread == 0){    //Main thread running
        for(th = threads; th < &threads[MAX_UTHREADS]; th++){
            if(th->state == RUNNABLE && first){   //First thread
                maxPrThread = th;
                max_priority = th->priority;
                first = 0;
            } else if(th->state == RUNNABLE && th->priority > max_priority) {  //Swap
                maxPrThread = th;
                max_priority = th->priority;
            }
        }
        return maxPrThread;

    } else {
        int curr_thread_index = get_pCurrThread_index();
        //Looks for the next thread in a round-robin manner, starting from the index of the running thread.
        for(int i = 1; i <= MAX_UTHREADS; i++) {

            th = &threads[(curr_thread_index+i) % MAX_UTHREADS];

            if(th == pCurrThread){    //We completed a full round-robin traversal of the array.
                break;
            } else if(th->state == RUNNABLE && first){   //First thread
                maxPrThread = th;
                max_priority = th->priority;
                first = 0;
            } else if(th->state == RUNNABLE && th->priority > max_priority) {  //Swap
                maxPrThread = th;
                max_priority = th->priority;
            }
        }

        return maxPrThread;
    }
}


void uthread_yield() {
    struct uthread *next = get_next_runnable_thread();
    //Determines whether there is another thread that can be executed, using the priority scheduling policy.
    if(next != 0 && next->priority >= pCurrThread-> priority){
        struct uthread *prev = pCurrThread;
        prev->state = RUNNABLE;
        next->state = RUNNING;
        pCurrThread = next;
        uswtch(&prev->context,&next->context);
    }
}


void uthread_exit() {
    struct uthread *next = get_next_runnable_thread();
    //Determines whether there is another thread that can be executed.
    if(next != 0){
        struct uthread *prev = pCurrThread;
        prev->state = FREE;
        pCurrThread = next;
        next->state = RUNNING;
        uswtch(&prev->context,&next->context);
    } else {
        pCurrThread->state = FREE;
        exit(0);
    } 
}

//Create and initialize new threads within our threads array.
int uthread_create(void (*start_func)(), enum sched_priority priority) {
    struct uthread *t;
    for(t = threads; t < &threads[MAX_UTHREADS]; t++) {       // Search for unused thread.
        if(t->state == FREE) {
            t->context.ra = (uint64) start_func;
            t->context.sp = (uint64) &t->ustack[STACK_SIZE-1];
            t->priority = priority;
            t->state = RUNNABLE;
            return 0;
        }
    }
    return -1;      // not found
}


enum sched_priority uthread_set_priority(enum sched_priority priority) {
    enum sched_priority prevPriority = uthread_self()->priority;
    uthread_self()->priority = priority;
    return prevPriority;
}


enum sched_priority uthread_get_priority() {
    return uthread_self()->priority;
}


int uthread_start_all() {
    static int first = 1;

    if(first) {
        struct context context;
        pCurrThread = get_next_runnable_thread();
        pCurrThread->state = RUNNING; 
        first = 0;
        uswtch(&context, &pCurrThread->context);
    }
    return -1;
}


struct uthread* uthread_self() {
    return pCurrThread;
}
