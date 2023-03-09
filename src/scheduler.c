#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
int fcfs()
{
    printf("Running FCFS scheduler\n");
    int errCode = 0;

    while (ready_queue_is_empty() != 1)
    {
        SCRIPT_PCB *current_pcb = dequeue_ready_queue();
        char *instruction = malloc(sizeof(char) * 100);
        int start_pos = current_pcb->start_pos;
        int script_len = current_pcb->script_len;
        char *name = current_pcb->name;
        int current_instruction = 0;
        while (current_instruction < script_len)
        {
            current_instruction = current_pcb->current_instruction;
            char *instruction = malloc(sizeof(char) * 100);
            errCode = get_instruction(instruction, name, start_pos, current_instruction, script_len);
            // printf("This is the instruction:%s \n", instruction); // debugging
            parseInput(instruction);
            free(instruction);
            increment_instruction(current_pcb);
        }
        mem_free_script(start_pos, script_len);
        free_script_pcb(current_pcb);
    }

    return errCode;
}
// For Shortest Job First, we use the number of lines of code in each program to estimate the job length.
int sjf()
{
    printf("Running SJF scheduler\n");
    int errCode = 0;

    // check which program is the shortest

    // run that program

    return errCode;
}

int rr()
{
    int errCode = 0;

    return errCode;
}

int aging()
{
    int errCode = 0;

    return errCode;
}

int startScheduler(char *policy)
{
    int errCode = 0;
    if (strcmp(policy, "FCFS") == 0)
    {
        errCode = fcfs();
    }
    else if (strcmp(policy, "RR") == 0)
    {
        errCode = rr();
    }
    else if (strcmp(policy, "SJF") == 0)
    {
        errCode = sjf();
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
