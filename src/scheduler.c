#include <stdio.h>
#include <stdlib.h>
#include "ready_queue.h"
#include "interpreter.h"
#include "pcb.h"
// Parts of this code were inspired from a combination of stackoverflow and chatGPT

// THIS CODE IS NOT COMPLETE. IT IS ONLY A SNIPPET OF THE CODE THAT WILL BE USED TO IMPLEMENT THE SCHEDULER.

READY_QUEUE *ready_queue = create_ready_queue();      // create the ready queue
SCRIPT_PCB *script_pcb = create_script_pcb(1, 0, 10); // create the SCRIPT PCB with PID 1, starting at memory location 0, and with 10 instructions
enqueue_ready_queue(ready_queue, script_pcb);         // add the SCRIPT PCB to the tail of the ready queue

int running = 1; // flag to indicate if there are still processes in the ready queue
while (running)
{
    SCRIPT_PCB *current_pcb = dequeue_ready_queue(ready_queue); // get the PCB at the head of the ready queue
    if (current_pcb == NULL)
    { // if there are no more PCBs in the ready queue, exit the loop
        running = 0;
        break;
    }

    int current_instruction = current_pcb->current_instruction;                        // get the current instruction of the PCB
    char *instruction = get_instruction(current_pcb->start_pos + current_instruction); // get the instruction from the interpreter
    execute_instruction(instruction);                                                  // execute the instruction
    free(instruction);                                                                 // free the memory allocated for the instruction

    current_pcb->current_instruction++; // move the current instruction pointer of the PCB to the next instruction
    if (current_pcb->current_instruction >= current_pcb->script_len)
    { // if the PCB has finished executing, free its memory and move on to the next process in the ready queue
        free_script_pcb(current_pcb);
    }
    else
    { // if the PCB has not finished executing, add it back to the tail of the ready queue
        enqueue_ready_queue(ready_queue, current_pcb);
    }
}

destroy_ready_queue(ready_queue); // free the memory allocated for the ready queue
