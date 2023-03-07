#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ready_queue.h"


struct PCB
{
    int pid;
    int address; //location in memory
    int length; //size of program (in lines)
    int pc; // current instruction
};

void initPCB(int address, int length)
{
    // Create empty PCB
    struct PCB *pcb = malloc(sizeof(struct PCB));

    // Set PCB values
    srand((unsigned int)time(NULL));
    pcb->pid = rand(); ;
    pcb->address = address;
    pcb->length = length;
    pcb->pc = 0;

    return;
}

int setPCB(int pid, int pc)
{
    // Find PCB with matching pid
    struct PCB *pcb = findPCB(pid); // in ready queue
    pcb->pc = 0;
    return pc;
}

