#include <stdlib.h>
#include "ready_queue.h"
#include "pcb.h"
// Parts of this code were inspired from a combination of stackoverflow and chatGPT

SCRIPT_PCB *find_shortest_job();
SCRIPT_PCB *get_ready_queue_head();

// Declare a global ready queue
READY_QUEUE *ready_queue;

// Function to initialize the global ready queue
void queue_init()
{
    // Create a new ready queue and assign it to the global variable
    ready_queue = create_ready_queue();
}

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
void enqueue_ready_queue(SCRIPT_PCB *pcb)
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
SCRIPT_PCB *dequeue_ready_queue()
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

int ready_queue_is_empty()
{
    return ready_queue->head == NULL;
}

// Function to free the memory allocated to a ready queue
void destroy_ready_queue()
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

// Function to get the head of the ready queue
SCRIPT_PCB *get_ready_queue_head()
{
    if (ready_queue->head == NULL)
    {
        return NULL;
    }
    return ready_queue->head;
}

// Function to find the shortest job in the ready queue
SCRIPT_PCB *find_shortest_job()
{
    // If the ready queue is empty, return null
    if (ready_queue->head == NULL)
    {
        return NULL;
    }
    // Otherwise, find the shortest job in the ready queue
    else
    {
        SCRIPT_PCB *current = ready_queue->head;
        SCRIPT_PCB *shortest = current;
        while (current != NULL)
        {
            if (current->job_length_score < shortest->job_length_score)
            {
                shortest = current;
            }
            current = current->next;
        }
        return shortest;
    }
}