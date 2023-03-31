#ifndef PAGING_H
#define PAGING_H

void init_backing_store();
void delete_backing_store();
int store_script(char *script);
int get_number_lines(char *script);
void load_page_from_disk(char *script, int num_frames);

#endif /* PAGING_H */
