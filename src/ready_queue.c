#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ready_queue.h"
#include "pcb.h"
// Parts of this code were inspired from a combination of stackoverflow and chatGPT

SCRIPT_PCB *find_shortest_job();
SCRIPT_PCB *get_ready_queue_head();
void move_to_front(SCRIPT_PCB *target);
int get_number_of_jobs_in_ready_queue();

// Declare a global ready queue
READY_QUEUE *ready_queue;

// Function to initialize  the global ready queue
void queue_init()
{
    ready_queue = create_ready_queue(); // Create a new ready queue and assign it to the global variable
}

// Function to create a new ready queue
READY_QUEUE *create_ready_queue()
{
    READY_QUEUE *ready_queue = (READY_QUEUE *)malloc(sizeof(READY_QUEUE));
    ready_queue->head = NULL;
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

// We are not using this function, but it might come in handy later  
// Function to add a new PCB in the right spot in the ready queue based on its job_length_score
void place_in_ready_queue(SCRIPT_PCB *pcb)
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
        while (current->next != NULL && current->next->job_length_score < pcb->job_length_score) // this is where the magic happens
        {
            current = current->next;
        }
        
        // assign to that spot and rearrage pointers
        SCRIPT_PCB *temp = current->next;
        current->next = pcb;
        pcb->next = temp;
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

// Function to peek at the first PCB in the ready queue
SCRIPT_PCB *peek_ready_queue()
{
    // If the ready queue is empty, return null
    if (ready_queue->head == NULL)
    {
        return NULL;
    }
    // Otherwise, return the head of the ready queue
    else
    {
        SCRIPT_PCB *head = ready_queue->head;
        return head;
    }
}

int ready_queue_is_empty()
{
    int is_empty = ready_queue->head == NULL;
    return is_empty;
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
    SCRIPT_PCB *tmp_job = NULL;
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
        SCRIPT_PCB *tmp1 = NULL;
        SCRIPT_PCB *tmp2 = NULL;
        SCRIPT_PCB *tmp3 = NULL;
        while (current != NULL)
        {
            if (current->job_length_score < shortest->job_length_score)
            {
                shortest = current;
            }
            // else // not sure if there is a need for this else statement
            current = current->next;
        }

        while (shortest != ready_queue->head) // this moves the shortest job to the front of the queue
        {
            // Dequeue the processes before the shortest job
            if (shortest != ready_queue->head)
            {
                tmp_job = dequeue_ready_queue();
                enqueue_ready_queue(tmp_job);
            }
        }

        return shortest;
    }
}

// Function to find the shortest job in the ready queue
void reorder_ready_queue()
{
    SCRIPT_PCB *tmp_job = NULL;
    // If the ready queue is empty, return null
    if (ready_queue->head == NULL)
        return;
    // Otherwise, find the shortest job in the ready queue
    else
    {
        SCRIPT_PCB *current = ready_queue->head;
        SCRIPT_PCB *shortest = current;
        SCRIPT_PCB *prev_job = NULL;

        while (current != NULL)
        {
            if (current->job_length_score < shortest->job_length_score)
            {
                prev_job = current;
                shortest = current;
            }
            current = current->next;
        }
        move_to_front(shortest); // move the shortest job to the front of the queue

        // Sort the queue by job length score
        if (get_number_of_jobs_in_ready_queue() == 3){
            SCRIPT_PCB *script2 = ready_queue->head->next;
            SCRIPT_PCB *script3 = ready_queue->head->next->next;
            if (script2->job_length_score > script3->job_length_score)
            {
                ready_queue->head->next = script3;
                script3->next = script2;
                script2->next = NULL;
            }
        }
        return;
    }

}

// Decrement the job length score of all jobs in the ready queue except the current job
void decrement_job_length_score(SCRIPT_PCB *current_job)
{
    SCRIPT_PCB *job_to_age = get_ready_queue_head();
    while (job_to_age != NULL)
    {
        if (job_to_age != current_job)
        {
            if (job_to_age->job_length_score > 0)
                job_to_age->job_length_score--;
        }
        job_to_age = job_to_age->next;
    }
}

void print_ready_queue()
{
    SCRIPT_PCB *tmp_job = NULL;
    printf("Ready queue:\n");
    for (tmp_job = get_ready_queue_head(); tmp_job != NULL; tmp_job = tmp_job->next)
    {
        printf("\t(%s, %d)\n", tmp_job->name, tmp_job->job_length_score);
    }
    printf("\n");
}

// dequeue a specific PCB and move it to the front of the ready queue
void move_to_front(SCRIPT_PCB *target)
{
    SCRIPT_PCB *current = ready_queue->head;
    SCRIPT_PCB *prev = NULL;

    // find the target PCB in the queue
    while (current != NULL && current != target)
    {
        prev = current;
        current = current->next;
    }

    // if the target PCB was found, remove it from its current position
    if (current == target)
    {
        if (prev == NULL)
        { // target is at the head of the queue
            return;
        }
        else
        {
            prev->next = target->next;
        }

        // move the target PCB to the front of the queue
        target->next = ready_queue->head;
        ready_queue->head = target;
    }
}

int get_number_of_jobs_in_ready_queue()
{
    int count = 0;
    SCRIPT_PCB *tmp_job = NULL;
    for (tmp_job = get_ready_queue_head(); tmp_job != NULL; tmp_job = tmp_job->next)
    {
        count++;
    }
    return count;
}