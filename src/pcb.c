#include <stdlib.h>
#include "pcb.h"
// Parts of this code were inspired from a combination of stackoverflow and chatGPT

// Function to create a new PCB with the given PID, start position, and script length
SCRIPT_PCB *create_script_pcb(int pid, int start_pos, int script_len)
{
    // Allocate memory for the new PCB
    SCRIPT_PCB *pcb = (SCRIPT_PCB *)malloc(sizeof(SCRIPT_PCB));

    // Set the fields of the new PCB
    pcb->pid = pid;
    pcb->start_pos = start_pos;
    pcb->script_len = script_len;
    pcb->current_instruction = start_pos;
    pcb->next = NULL;

    // Return the new PCB
    return pcb;
}

// Function to free the memory allocated to a PCB
void free_script_pcb(SCRIPT_PCB *pcb)
{
    free(pcb);
}
