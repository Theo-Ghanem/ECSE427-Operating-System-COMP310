#include "pcb.h"

typedef struct ready_queue
{
    SCRIPT_PCB *head;
} READY_QUEUE;

READY_QUEUE *create_ready_queue();
void enqueue_ready_queue(READY_QUEUE *ready_queue, SCRIPT_PCB *pcb);
SCRIPT_PCB *dequeue_ready_queue(READY_QUEUE *ready_queue);
void destroy_ready_queue(READY_QUEUE *ready_queue);
