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
int MT;

// get the instruction from memory
int get_instruction(char *instruction, char *name, int start_pos, int current_instruction, int script_len)
{
    int errCode = 0;

    if (current_instruction >= script_len)
    {
        errCode = 1;
        return errCode;
    }

    // get intruction from memory according to the name of the script and the current instruction
    // the instruction is stored in the memory as "name_current_instruction"
    char instr[100];
    sprintf(instr, "%s_%d", name, current_instruction);
    char *token = mem_get_value(instr);

    if (token == NULL)
    {
        errCode = 1;
        return errCode;
    }
    else if (strcmp(token, "Variable does not exist") == 0)
    {
        errCode = 1;
    }
    else
    {
        strcpy(instruction, token);
    }

    return errCode;
}

int get_instruction_with_page_table(SCRIPT_PCB *pcb, char *instruction, char *name, int current_instruction, int script_len)
{
    int errCode = 0;

    if (current_instruction >= script_len)
    {
        errCode = 1;
        return errCode;
    }

    int current_page = current_instruction / 3;
    int page_offset = current_instruction % 3;

    int frame_index = pcb->page_table[current_page];
    
    // if frame_index is -1, then the page is not in memory :(
    if (frame_index == -1)
    {
        // load page from disk & update page table
        load_page_from_disk(name, 1, pcb);

        // update frame_index
        frame_index = pcb->page_table[current_page];
    }

    // get instruction from page according to the page table
    char *token = mem_get_value_at_index(frame_index*3 + page_offset);


    if (token == NULL)
    {
        errCode = 1;
        return errCode;
    }
    else if (strcmp(token, "Variable does not exist") == 0)
    {
        errCode = 1;
    }
    else
    {
        strcpy(instruction, token);
    }

    // update lru linked list
    // move_to_tail(frame_index);
    // remove_from_lru(frame_index);
    // add_to_lru(frame_index);
    


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
        // retrieve info from PCB
        SCRIPT_PCB *current_pcb = dequeue_ready_queue();
        int start_pos = current_pcb->start_pos;
        int script_len = current_pcb->script_len;
        char *name = current_pcb->name;
        int current_instruction = -1;
        while (current_instruction < script_len - 1)
        {
            // exectute all instructions in the script from memory
            current_instruction = current_pcb->current_instruction;
            char *instruction = malloc(sizeof(char) * 100);
            errCode = get_instruction_with_page_table(current_pcb, instruction, name, current_instruction, script_len);
            parseInput(instruction);
            free(instruction);
            increment_instruction(current_pcb);
        }
        // clear memory when the process is complete
        mem_free_script(start_pos, script_len);
        free_script_pcb(current_pcb);
    }
    return errCode;
}

//TODO modify this
// Round Robin scheduling policy works for any delta (2 or 30)
int rr(int delta)
{
    // printf("starting rr\n");
    int errCode = 0;

    while (ready_queue_is_empty() == 0)
    {
        // get first process from ready queue
        // lock the ready queue if in multi-threaded mode
        if (MT == 1)
            pthread_mutex_lock(&(pool->queue_lock));
        SCRIPT_PCB *current_pcb = dequeue_ready_queue();
        if (MT == 1)
            pthread_mutex_unlock(&(pool->queue_lock));

        // retrieve info from PCB
        int start_pos = current_pcb->start_pos;
        int script_len = current_pcb->script_len;
        char *name = current_pcb->name;
        int current_instruction = current_pcb->current_instruction;
        while (current_instruction < script_len)
        {
            // printf("current instruction: %d\n", current_instruction);
            // execute delta instructions from memory
            current_instruction = current_pcb->current_instruction;
            char *instruction = malloc(sizeof(char) * 100);
            errCode = get_instruction_with_page_table(current_pcb, instruction, name, current_instruction, script_len);
            // printf("instruction: %s\n", instruction);
            errCode = parseInput(instruction);
            free(instruction);
            increment_instruction(current_pcb);
            current_instruction = current_pcb->current_instruction;
            if (current_instruction % delta == 0 && current_instruction < script_len)
            {
                // lock the ready queue if in multi-threaded mode
                // enqueue the process back to the ready queue if not completed
                if (MT == 1)
                    pthread_mutex_lock(&(pool->queue_lock));
                enqueue_ready_queue(current_pcb);
                if (MT == 1)
                    pthread_mutex_unlock(&(pool->queue_lock));
                break;
            }
            
        }
        // only clear memory when the process is complete
        if (current_instruction >= script_len)
        {
            if (MT == 1)
                pthread_mutex_unlock(&(pool->queue_lock));
            // mem_free_script(start_pos, script_len);
            free_script_pcb(current_pcb);
            if (MT == 1)
                pthread_mutex_unlock(&(pool->queue_lock));
        }
    }

    return errCode;
}

// Aging scheduling policy
int aging()
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
        parseInput(instruction);
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
    MT = 0;
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
int startSchedulerMT(char *policy) //
{
    pol = policy;

    pthread_mutex_lock(&(pool->lock));

    // set the MT flag to 1 for multi-threaded mode
    MT = 1;

    // send the worker threads off by boradcasting a signal
    pthread_cond_broadcast(&(pool->work_ready));

    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

void *worker_thread_func(void *arg)
{
    thread_pool_t *pool = (thread_pool_t *)arg;

    pthread_mutex_lock(&(pool->lock));

    pthread_cond_wait(&(pool->work_ready), &(pool->lock));

    pthread_mutex_unlock(&(pool->lock));

    // printf("Thread starting scheduler\n");
    startScheduler(pol);
    // printf("Thread finished scheduler\n");

    return NULL;
}

void init_thread_pool(thread_pool_t *pl)
{
    pool = pl;
    MT = 0;

    // initialize mutex and condition variable objects for multithreading mode
    pthread_cond_init(&(pool->work_ready), NULL);
    pthread_mutex_init(&(pool->lock), NULL);
    pthread_mutex_init(&(pool->queue_lock), NULL);

    // create worker threads
    for (int i = 0; i < 2; i++)
    {
        pthread_create(&(pool->threads[i]), NULL, &worker_thread_func, (void *)pool);
    }
}

void wait_for_threads()
{
    // wait for worker threads to finish unless it is the worker thread itself
    for (int i = 0; i < 2; i++)
    {
        if (pool->threads[i] != pthread_self())
            pthread_join(pool->threads[i], NULL);
    }
}