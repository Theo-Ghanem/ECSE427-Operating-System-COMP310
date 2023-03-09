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

    char *instr = strcat(name, "_") + current_instruction;

    char* token = mem_get_value(instr);

    if (token == NULL)
    {
        errCode = 1;
    }
    else
    {
        strcpy(instruction, token);
    }

    return errCode;
}


// First come first serve scheduling policy
// runs processes in the order they arrive until all processes are complete
int fcfs()
{
    printf("Running FCFS scheduler");
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
            parseInput(instruction);
            free(instruction);
            increment_instruction(current_pcb);
        }
        mem_free_script(start_pos, script_len);
    }

    return errCode;
}

int rr()
{
    int errCode = 0;

    return errCode;
}

int sjf()
{
    int errCode = 0;

    return errCode;
}

int aging()
{
    int errCode = 0;

    return errCode;
}

int startScheduler(char* policy)
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
