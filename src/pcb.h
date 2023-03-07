typedef struct script_pcb
{
    int pid;
    int start_pos;
    int script_len;
    int current_instruction;
    struct script_pcb *next;
} SCRIPT_PCB;

SCRIPT_PCB *create_script_pcb(int pid, int start_pos, int script_len);
