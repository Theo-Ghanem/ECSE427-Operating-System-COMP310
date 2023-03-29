#ifndef READY_QUEUE_H
#define READY_QUEUE_H

#include "pcb.h"

// extern READY_QUEUE *ready_queue; //global variable

typedef struct ready_queue
{
    SCRIPT_PCB *head;
} READY_QUEUE;

void queue_init();
READY_QUEUE *create_ready_queue();
void enqueue_ready_queue(SCRIPT_PCB *pcb);
SCRIPT_PCB *dequeue_ready_queue();
void destroy_ready_queue();
int ready_queue_is_empty();
SCRIPT_PCB *get_ready_queue_head();
SCRIPT_PCB *find_shortest_job();
void reorder_ready_queue();
void decrement_job_length_score(SCRIPT_PCB *current_job);
SCRIPT_PCB *peek_ready_queue();
void print_ready_queue();
void move_to_front(SCRIPT_PCB *target);
void place_in_ready_queue(SCRIPT_PCB *pcb);
SCRIPT_PCB *find_pcb_in_ready_queue(char *name);

#endif // READY_QUEUE_H