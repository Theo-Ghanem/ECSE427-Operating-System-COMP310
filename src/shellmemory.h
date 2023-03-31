void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int mem_load_script(char *script, int *memLocation, int *memSize);
int mem_free_script(int memLocation, int memSize);
char *mem_get_value_at_index(int index);
void move_to_tail(int frame_index);
void load_page_from_disk(char *script, int num_frames);
