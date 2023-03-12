#include <stdlib.h>
#include "pcb.h"
#include "ready_queue.h"

// Parts of this code were inspired from a combination of stackoverflow and chatGPT

// Function to create a new PCB with the given PID, start position, and script length
int pid_counter = 0;

int create_script_pcb(SCRIPT_PCB *pcb, char *name, int start_pos, int script_len)
{
    // Set the fields of the new PCB
    pcb->pid = pid_counter++;
    pcb->name = name;
    pcb->start_pos = start_pos;
    pcb->script_len = script_len;
    pcb->current_instruction = 0;
    pcb->job_length_score = script_len; // Initialize job length score to script length
    pcb->next = NULL;

    // Add the new PCB to the global ready queue
    enqueue_ready_queue(pcb);

    return 0;
}

// Function to free the memory allocated to a PCB
void free_script_pcb(SCRIPT_PCB *pcb)
{
    free(pcb);
    pcb = NULL; //! added this, if we ever have problems refer back to this
}

int increment_instruction(SCRIPT_PCB *pcb)
{
    int instr = pcb->current_instruction;
    pcb->current_instruction = ++instr;
    return 0;
}
