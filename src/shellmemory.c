#define FRAME_SIZE 3

#ifndef FRAME_STORE_SIZE
#define FRAME_STORE_SIZE 600
#define VAR_STORE_SIZE 500
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "ready_queue.h"
#include "pcb.h"

struct memory_struct
{
	char *var;
	char *value;
};

struct memory_struct shellmemory[1000];
int var_mem_start = FRAME_STORE_SIZE + 1;
int var_mem_end = 1000;
int var_count = 0;								 // number of variables currently stored in the variable store
int var_index = FRAME_STORE_SIZE + 1;			 // index of the first available line in the variable store
int frame_index = 0;							 // index of the next available frame in the frame store
char *frame_store[FRAME_STORE_SIZE][FRAME_SIZE]; // array of arrays representing the frame store

// Helper functions
int match(char *model, char *var)
{
	int i, len = strlen(var), matchCount = 0;
	for (i = 0; i < len; i++)
		if (*(model + i) == *(var + i))
			matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model)
{
	char token = '='; // look for this to find value
	char value[1000]; // stores the extract value
	int i, j, len = strlen(model);
	for (i = 0; i < len && *(model + i) != token; i++)
		; // loop till we get there
	// extract the value
	for (i = i + 1, j = 0; i < len; i++, j++)
		value[j] = *(model + i);
	value[j] = '\0';
	return strdup(value);
}

// Shell memory functions

void mem_init()
{

	int i;
	for (i = 0; i < 1000; i++)
	{
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in)
{

	if (var_count >= var_mem_end - var_mem_start) // if the variable store is full
	{
		printf("Variable store full, unable to store variable %s.\n", var_in);
		return;
	}

	for (int i = var_mem_start; i < var_mem_end; i++)
	{
		if (strcmp(shellmemory[i].var, var_in) == 0) // if the variable already exists
		{
			shellmemory[i].value = strdup(value_in); // update the value
			return;
		}
	}

	// Value does not exist, need to find a free spot.
	for (int i = var_mem_start; i < var_mem_end; i++)
	{
		if (strcmp(shellmemory[i].var, "none") == 0) // if the spot is empty
		{
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			var_count++;
			var_index++;
			return;
		}
	}

	return;
}

// get value based on input key
char *mem_get_value(char *var_in)
{
	int i;

	for (i = var_mem_start; i < var_mem_end; i++)
	{
		if (strcmp(shellmemory[i].var, var_in) == 0)
		{

			return strdup(shellmemory[i].value);
		}
	}
	return "Variable does not exist";
}

// helper function to find free frame in memory
int find_free_frame()
{
	for (int i = 0; i < var_mem_start; i += 3)
	{
		if (strcmp(shellmemory[i].var, "none") == 0)
		{
			return i / 3;
		}
	}
	return -1;
}

// load specified number of frames from script in memory starting at current frame
// a negative number of frames will load all frames from the script
void load_page_from_disk(char *script, int num_frames)
{
	SCRIPT_PCB *pcb = find_pcb_in_ready_queue(script);
	if (pcb == NULL)
	{
		printf("Script not found in ready queue.\n");
		return;
	}

	int script_size = pcb->script_len;
	int current_instruction = pcb->current_instruction;
	int current_page = current_instruction / 3;
	int max_num_pages = pcb->num_pages;
	int *page_table = pcb->page_table;

	if (num_frames < 0)
	{
		num_frames = max_num_pages;
	}
	else if (num_frames + current_page > max_num_pages)
	{
		// safely fix the value; invisible to the user
		num_frames = max_num_pages - current_page;
	}

	// open the script file
	char *script_disk = strcat("backing_store/", script);
	FILE *p = fopen(script_disk, "rt");

	if (p == NULL) // if the file does not exist
	{
		printf("File does not exist\n");
		return;
	}
	char line[100];

	// skip to the current page
	for (int i = 0; i < current_page; i++)
	{
		fgets(line, 99, p);
	}

	// load the specified number of frames from the script
	for (int i = current_page; i < num_frames; i++)
	{
		// find free frame in memory
		int frame_index = find_free_frame();
		if (frame_index == -1)
		{
			printf("No free frames in memory.\n");
			return;
		}

		// write UP TO 3 instructions to the frame
		for (int j = 0; j < 3 && current_instruction <= script_size; j++)
		{
			// create unique ID for program line
			// format: scriptname_page#_line#
			char name[100];
			sprintf(name, "%s_page%d_line%d", script, i, current_instruction);

			fgets(line, 99, p); // read the next line of the file

			shellmemory[frame_index * 3 + j].var = strdup(name);
			shellmemory[frame_index * 3 + j].value = strdup(line);

			memset(line, 0, sizeof(line)); // empty the string
			memset(name, 0, sizeof(name));

			current_instruction++;
		}

		fclose(p);
	}
}

// A2 1.2.1 Freeing memory
int mem_free_script(int memLocation, int memSize)
{
	int errCode = 0;

	for (int i = memLocation; i < memLocation + memSize; i++)
	{
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}

	return errCode;
}