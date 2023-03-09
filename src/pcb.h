#ifndef SCRIPT_PCB_H
#define SCRIPT_PCB_H

typedef struct script_pcb
{
    int pid;
    int start_pos;
    int script_len;
    char *name;
    int current_instruction;
    struct script_pcb *next;
} SCRIPT_PCB;

int create_script_pcb(SCRIPT_PCB *pcb, char *name, int start_pos, int script_len);
int increment_instruction(SCRIPT_PCB *pcb);
void free_script_pcb(SCRIPT_PCB *pcb);

#endif // SCRIPT_PCB_H
