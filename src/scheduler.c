#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "ready_queue.h"
#include "shell.h"
#include "pcb.h"
#include "shellmemory.h"
#include "scheduler.h"

int run_scheduler = 0;
char *pol;
thread_pool_t *pool;
int end;

int get_instruction(char *instruction, char *name, int start_pos, int current_instruction, int script_len)
{
    int errCode = 0;

    if (current_instruction >= script_len)
    {
        errCode = 1;
        return errCode;
    }

    char instr[100];
    sprintf(instr, "%s_%d", name, current_instruction);
    char *token = mem_get_value(instr);

    if (token == NULL)
    {
        errCode = 1;
        return errCode;
    }
    else
    {
        strcpy(instruction, token);
    }

    return errCode;
}

// First come first serve scheduling policy
// runs processes in the order they arrive until all processes are complete
// This is also used for Shortest Job First, we enqued the processes based on their job length.
int fcfs()
{
    int errCode = 0;

    while (ready_queue_is_empty() != 1)
    {
        SCRIPT_PCB *current_pcb = dequeue_ready_queue();
        int start_pos = current_pcb->start_pos;
        int script_len = current_pcb->script_len;
        char *name = current_pcb->name;
        int current_instruction = -1;
        while (current_instruction < script_len - 1)
        {
            current_instruction = current_pcb->current_instruction;
            char *instruction = malloc(sizeof(char) * 100);
            errCode = get_instruction(instruction, name, start_pos, current_instruction, script_len);
            parseInput(instruction);
            free(instruction);
            increment_instruction(current_pcb);
        }
        mem_free_script(start_pos, script_len);
        free_script_pcb(current_pcb);
    }
    return errCode;
}

// Round Robin scheduling policy
int rr(int delta)
{
    int errCode = 0;

    while (ready_queue_is_empty() == 0)
    {
        SCRIPT_PCB *current_pcb = dequeue_ready_queue();
        int start_pos = current_pcb->start_pos;
        int script_len = current_pcb->script_len;
        char *name = current_pcb->name;
        int current_instruction = current_pcb->current_instruction;
        while (current_instruction < script_len)
        {
            current_instruction = current_pcb->current_instruction;
            char *instruction = malloc(sizeof(char) * 100);
            errCode = get_instruction(instruction, name, start_pos, current_instruction, script_len);
            parseInput(instruction);
            free(instruction);
            increment_instruction(current_pcb);
            current_instruction = current_pcb->current_instruction;
            if (current_instruction % delta == 0 && current_instruction < script_len)
            {
                enqueue_ready_queue(current_pcb);
                break;
            }
        }
        // only clear memory when the process is complete
        if (current_instruction >= script_len)
        {
            mem_free_script(start_pos, script_len);
            free_script_pcb(current_pcb);
        }
    }

    return errCode;
}

// Aging scheduling policy
int aging() // original one
{
    int errCode = 0;
    SCRIPT_PCB *current_job = peek_ready_queue(); // Get the next job to run
    SCRIPT_PCB *tmp_job = NULL;
    while (current_job != NULL && get_ready_queue_head() != NULL)
    {
        // Run the current job for one instruction
        int current_instruction = current_job->current_instruction;
        char *instruction = malloc(sizeof(char) * 100);
        errCode = get_instruction(instruction, current_job->name, current_job->start_pos, current_instruction, current_job->script_len);
        errCode = parseInput(instruction);
        free(instruction);
        increment_instruction(current_job);

        // Check if the current job is finished
        if (current_job->current_instruction == current_job->script_len)
        {
            dequeue_ready_queue();
            mem_free_script(current_job->start_pos, current_job->script_len);
            free_script_pcb(current_job);
            current_job = peek_ready_queue();
        }
        else
        {
            // Decrement the job length score of all jobs in the ready queue except the current job
            decrement_job_length_score(current_job);

            // Reorder the ready queue based on the job length score
            reorder_ready_queue();

            // Get the next job to run
            current_job = peek_ready_queue();
        }
    }
    return errCode;
}

// This function starts the scheduler based on the policy passed in
void startScheduler(char *policy)
{
    int errCode = 0;
    if (strcmp(policy, "FCFS") == 0)
    {
        errCode = fcfs();
    }
    else if (strcmp(policy, "RR") == 0)
    {
        errCode = rr(2);
    }
    else if (strcmp(policy, "RR30") == 0)
    {
        errCode = rr(30);
    }
    else if (strcmp(policy, "SJF") == 0)
    {
        errCode = fcfs(); // this is because we already sorted the ready queue based on job length
    }
    else if (strcmp(policy, "AGING") == 0)
    {
        errCode = aging();
    }
    else
    {
        printf("Invalid scheduling policy. Please try again.\n");
        errCode = 1;
    }
}

// Part 3 Multithreaded scheduler
void *poll_scheduler(void *arg) //
{
    while (run_scheduler == 0)
    {
    }
    char *policy = (char *)arg;
    startScheduler(policy);
    return NULL;
}

int startSchedulerMT(char *policy) //
{
    pol = policy;
    pthread_mutex_lock(&(pool->lock));
    pool->work_to_do = 2;
    pthread_cond_broadcast(&(pool->work_ready));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

void *worker_thread_func(void *arg)
{

    thread_pool_t *pool = (thread_pool_t *)arg;
    while (end == 0)
    {
        pthread_mutex_lock(&(pool->lock));

        while (pool->work_to_do == 0)
        {
            pthread_cond_wait(&(pool->work_ready), &(pool->lock));
            if (end == 1)
            {
                pthread_mutex_unlock(&(pool->lock));
                return NULL;
            }
        }
        pool->work_to_do = --pool->work_to_do;
        pthread_mutex_unlock(&(pool->lock));

        startScheduler(pol);
    }
    return NULL;
}

void init_thread_pool(thread_pool_t *pl)
{
    int i;
    pool = pl;
    end = 0;
    pool->work_to_do = 0;
    pthread_cond_init(&(pool->work_ready), NULL);
    pthread_mutex_init(&(pool->lock), NULL);
    for (i = 0; i < 2; i++)
    {
        pthread_create(&(pool->threads[i]), NULL, &worker_thread_func, (void *)pool);
    }
}

void end_it_all()
{
    end = 1;
}