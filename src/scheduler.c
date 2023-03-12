#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ready_queue.h"
#include "shell.h"
#include "pcb.h"
#include "shellmemory.h"

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

    // printf("Var name: %s\n", instr);
    char *token = mem_get_value(instr);
    // printf("Token: %s\n", token);

    if (token == NULL)
    {
        errCode = 1;
        return errCode;
    }
    else
    {
        strcpy(instruction, token);
        // printf("Instruction: %s\n", instruction);
    }

    return errCode;
}

// First come first serve scheduling policy
// runs processes in the order they arrive until all processes are complete
// This is also used for Shortest Job First, we enqued the processes based on their job length.
int fcfs()
{
    // printf("Running FCFS scheduler\n");
    int errCode = 0;

    while (ready_queue_is_empty() != 1)
    {
        SCRIPT_PCB *current_pcb = dequeue_ready_queue();
        int start_pos = current_pcb->start_pos;
        int script_len = current_pcb->script_len;
        char *name = current_pcb->name;
        int current_instruction = 0;
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

    while (ready_queue_is_empty() != 1)
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
// int aging()
// {
//     // printf("Running Aging scheduler\n");
//     int errCode = 0;
//     SCRIPT_PCB *current_job = dequeue_ready_queue(); // Get the next job to run
//     SCRIPT_PCB *new_job = NULL;
//     SCRIPT_PCB *job_to_age = NULL;
//     SCRIPT_PCB *tmp_job = NULL;

//     while (1)
//     {
//         // print for debugging
//         printf("(%s, %d) ", current_job->name, current_job->job_length_score);
//         for (tmp_job = get_ready_queue_head(); tmp_job != NULL; tmp_job = tmp_job->next)
//         {
//             printf("(%s, %d) ", tmp_job->name, tmp_job->job_length_score);
//         }
//         printf("\n");
//         if (current_job != NULL)
//         {

//             // Run the current job for one instruction
//             printf("----------------------------------------\n");
//             int current_instruction = 0;
//             current_instruction = current_job->current_instruction;
//             char *instruction = malloc(sizeof(char) * 100);
//             errCode = get_instruction(instruction, current_job->name, current_job->start_pos, current_instruction, current_job->script_len);
//             parseInput(instruction);
//             free(instruction);
//             increment_instruction(current_job);
//             // printf("^ the job is %s, the length score is %d \n", current_job->name, current_job->job_length_score);
//             // printf("(%s, %d)\n", current_job->name, current_job->job_length_score);

//             // Check if the current job is finished
//             if (current_job->current_instruction == current_job->script_len)
//             {
//                 free_script_pcb(current_job);
//                 current_job = dequeue_ready_queue();
//             }

//             // Check if there is a new job in the ready queue with a lower job length score
//             new_job = find_shortest_job();
//             if (new_job != NULL)
//             {
//                 // Preempt the current job and run the new job
//                 enqueue_ready_queue(current_job);
//                 current_job = dequeue_ready_queue();
//             }
//         }

//         // Age the jobs in the ready queue
//         job_to_age = get_ready_queue_head();
//         while (job_to_age != NULL && job_to_age != current_job)
//         {
//             job_to_age->job_length_score = job_to_age->job_length_score - 1;
//             if (job_to_age->job_length_score < 0)
//             {
//                 job_to_age->job_length_score = 0;
//             }
//             job_to_age = job_to_age->next;
//         }

//         // Check if the current job is still the shortest job in the ready queue
//         new_job = find_shortest_job();
//         if (new_job != NULL)
//         {
//             // Preempt the current job and run the new job
//             enqueue_ready_queue(current_job);
//             current_job = dequeue_ready_queue();
//         }

//         // Check if there are any jobs left in the ready queue
//         if (current_job == NULL)
//         {
//             break;
//         }
//     }
//     return errCode;
// }

// draft for aging:
// int aging()
// {
//     int errCode = 0;
//     SCRIPT_PCB *current_job = NULL;
//     SCRIPT_PCB *job_to_age = NULL;

//     // Get the next job to run and reorder the ready queue based on job_length_score
//     current_job = dequeue_ready_queue();
//     age_ready_queue();

//     while (current_job != NULL)
//     {
//         // Run the current job for one instruction
//         printf("----------------------------------------\n");
//         int current_instruction = current_job->current_instruction;
//         char *instruction = malloc(sizeof(char) * 100);
//         errCode = get_instruction(instruction, current_job->name, current_job->start_pos, current_instruction, current_job->script_len);
//         parseInput(instruction);
//         free(instruction);
//         increment_instruction(current_job);

//         // Check if the current job is finished
//         if (current_job->current_instruction == current_job->script_len)
//         {
//             free_script_pcb(current_job);
//         }
//         else
//         {
//             // Decrement the job_length_score of all jobs in the ready queue except the current one
//             job_to_age = get_ready_queue_head();
//             while (job_to_age != NULL)
//             {
//                 if (job_to_age != current_job)
//                 {
//                     job_to_age->job_length_score--;
//                     if (job_to_age->job_length_score < 0)
//                     {
//                         job_to_age->job_length_score = 0;
//                     }
//                 }
//                 job_to_age = job_to_age->next;
//             }

//             // Reorder the ready queue based on job_length_score
//             enqueue_ready_queue(current_job);
//             current_job = dequeue_ready_queue();
//             age_ready_queue();
//         }
//     }
//     return errCode;
// }

// other draft
int aging()
{
    int errCode = 0;
    SCRIPT_PCB *current_job = peek_ready_queue(); // Get the next job to run

    while (current_job != NULL)
    {
        // Run the current job for one instruction
        // printf("----------------------------------------\n");

        int current_instruction = current_job->current_instruction;
        char *instruction = malloc(sizeof(char) * 100);
        errCode = get_instruction(instruction, current_job->name, current_job->start_pos, current_instruction, current_job->script_len);
        parseInput(instruction);
        free(instruction);
        increment_instruction(current_job);

        // Check if the current job is finished
        if (current_job->current_instruction == current_job->script_len)
        {
            free_script_pcb(current_job);
            dequeue_ready_queue();
            current_job = peek_ready_queue();
            current_job->job_length_score--;
            if (current_job->job_length_score < 0)
            {
                current_job->job_length_score = 0;
            }
        }
        else
        {
            // Decrement the job length score of all jobs in the ready queue except the current job
            SCRIPT_PCB *job_to_age = get_ready_queue_head();
            while (job_to_age != NULL)
            {
                if (job_to_age != current_job)
                {
                    job_to_age->job_length_score--;
                    if (job_to_age->job_length_score < 0)
                    {
                        job_to_age->job_length_score = 0;
                    }
                }
                job_to_age = job_to_age->next;
            }

            // Reorder the ready queue based on the job length score
            reorder_ready_queue();

            // Get the next job to run
            current_job = peek_ready_queue();
        }
    }

    return errCode;
}

// This function starts the scheduler based on the policy passed in
int startScheduler(char *policy)
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

    return errCode;
}
