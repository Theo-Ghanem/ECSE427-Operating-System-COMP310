void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
int mem_load_script(char *script, int *memLocation, int *memSize);
int mem_free_script(int memLocation, int memSize);
