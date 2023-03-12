#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "ready_queue.h"

// Function to run the scheduler
void startScheduler(char *pol);
int startSchedulerMT(char *pol);
#endif