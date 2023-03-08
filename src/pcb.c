#include <stdlib.h>
#include "pcb.h"
// Parts of this code were inspired from a combination of stackoverflow and chatGPT

// Function to create a new PCB with the given PID, start position, and script length
int pid_counter = 0;

int *create_script_pcb(SCRIPT_PCB *pcb, int start_pos, int script_len)
{
    // Set the fields of the new PCB
    pcb->pid = pid_counter++;
    pcb->start_pos = start_pos;
    pcb->script_len = script_len;
    pcb->current_instruction = 0;
    pcb->next = NULL;
    
    return 0;
}

// Function to free the memory allocated to a PCB
void free_script_pcb(SCRIPT_PCB *pcb)
{
    free(pcb);
}
