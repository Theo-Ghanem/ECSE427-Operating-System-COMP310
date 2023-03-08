typedef struct script_pcb
{
    int pid;
    int start_pos;
    int script_len;
    int current_instruction;
    struct script_pcb *next;
} SCRIPT_PCB;

int *create_script_pcb(SCRIPT_PCB *pcb, int start_pos, int script_len);
