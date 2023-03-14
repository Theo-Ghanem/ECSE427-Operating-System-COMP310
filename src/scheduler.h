#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "ready_queue.h"

typedef struct thread_pool
{
    pthread_t threads[2];
    pthread_cond_t work_ready;
    pthread_mutex_t lock;
    pthread_mutex_t queue_lock;
} thread_pool_t;

// Function to run the scheduler
void startScheduler(char *pol);
int startSchedulerMT(char *pol);
void init_thread_pool(thread_pool_t *pl);
void wait_for_threads();
#endif