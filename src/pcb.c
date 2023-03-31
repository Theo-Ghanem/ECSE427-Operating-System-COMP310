#include <stdlib.h>
#include <string.h>
#include "pcb.h"
#include "ready_queue.h"

// Parts of this code were inspired from a combination of stackoverflow and chatGPT

// Function to create a new PCB with the given PID, start position, and script length
int pid_counter = 0;

int create_script_pcb(SCRIPT_PCB *pcb, char *name, int num_lines)
{
    // Set the fields of the new PCB
    pcb->pid = pid_counter++;
    pcb->name = name;
    pcb->start_pos = 0;          //! this is not used in A3 and should be removed
    pcb->script_len = num_lines; //! this is not used in A3 and should be removed
    pcb->current_instruction = 0;
    pcb->job_length_score = 0; //! this is not used in A3 and should be removed
    pcb->next = NULL;

    int num_pages = num_lines / 3;
    if (num_lines % 3 != 0)
    {
        num_pages++;
    }
    pcb->num_pages = num_pages;

    // page table, make an array of ints
    // int temp[num_pages];
    // memcpy(pcb->page_table, temp, sizeof(temp));

    // initialize page table to -1 for all values
    for (int i = 0; i < num_pages; i++)
    {
        pcb->page_table[i] = -1;
    }

    // Add the new PCB to the global ready queue
    enqueue_ready_queue(pcb);

    return 0;
}

// Function to free the memory allocated to a PCB
void free_script_pcb(SCRIPT_PCB *pcb)
{
    free(pcb);
    pcb = NULL;
}

// Function to increment the current instruction of a PCB
int increment_instruction(SCRIPT_PCB *pcb)
{
    int instr = pcb->current_instruction;
    pcb->current_instruction = ++instr;
    return 0;
}
