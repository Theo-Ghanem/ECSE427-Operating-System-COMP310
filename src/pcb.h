#ifndef SCRIPT_PCB_H
#define SCRIPT_PCB_H

typedef struct script_pcb
{
    int pid;
    int start_pos;
    int script_len;
    char *name;
    int current_instruction;
    int job_length_score; // new field to keep track of job length score
    struct script_pcb *next;
    int num_pages;
    int page_table[];
} SCRIPT_PCB;

int create_script_pcb(SCRIPT_PCB *pcb, char *name, int num_lines);
int increment_instruction(SCRIPT_PCB *pcb);
void free_script_pcb(SCRIPT_PCB *pcb);

#endif // SCRIPT_PCB_H
