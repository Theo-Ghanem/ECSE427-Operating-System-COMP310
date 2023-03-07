#include <stdlib.h>
#include "ready_queue.h"
// Parts of this code were inspired from a combination of stackoverflow and chatGPT

// Function to create a new ready queue
READY_QUEUE *create_ready_queue()
{
    // Allocate memory for the new ready queue
    READY_QUEUE *ready_queue = (READY_QUEUE *)malloc(sizeof(READY_QUEUE));

    // Initialize the ready queue with a null head
    ready_queue->head = NULL;

    // Return the new ready queue
    return ready_queue;
}

// Function to add a new PCB to the end of the ready queue
void enqueue_ready_queue(READY_QUEUE *ready_queue, SCRIPT_PCB *pcb)
{
    // If the ready queue is empty, set the head to the new PCB
    if (ready_queue->head == NULL)
    {
        ready_queue->head = pcb;
    }
    // Otherwise, traverse the ready queue and add the new PCB to the end
    else
    {
        SCRIPT_PCB *current = ready_queue->head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = pcb;
    }
}

// Function to remove and return the first PCB in the ready queue
SCRIPT_PCB *dequeue_ready_queue(READY_QUEUE *ready_queue)
{
    // If the ready queue is empty, return null
    if (ready_queue->head == NULL)
    {
        return NULL;
    }
    // Otherwise, remove and return the head of the ready queue
    else
    {
        SCRIPT_PCB *head = ready_queue->head;
        ready_queue->head = head->next;
        head->next = NULL;
        return head;
    }
}

// Function to free the memory allocated to a ready queue
void destroy_ready_queue(READY_QUEUE *ready_queue)
{
    // Free all the PCBs in the ready queue
    SCRIPT_PCB *current = ready_queue->head;
    while (current != NULL)
    {
        SCRIPT_PCB *next = current->next;
        free(current);
        current = next;
    }

    // Free the ready queue
    free(ready_queue);
}