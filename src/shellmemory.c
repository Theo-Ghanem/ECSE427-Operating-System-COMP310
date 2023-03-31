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


int get_lru_page();
void add_node();
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

// get value at index
char *mem_get_value_at_index(int index)
{
	if (index > var_mem_start || index >= 0)
	{
		return "Index out of bounds";
	}
	return strdup(shellmemory[index].value);
}

// helper function to find free frame in memory
int find_free_frame()
{
	for (int i = 0; i < var_mem_start; i += 3)
	{
		
		if (strcmp(shellmemory[i].var, "none") == 0 || strcmp(shellmemory[i].value, "none") == 0)
		{
			return i / 3;
		}
		else
		{
			int victim_frame = get_lru_page();

			printf("Page fault! Victim page contents:\n");
			
			// print the contents of the victim page
			
			for (int j = 0; j < FRAME_SIZE; j++)
			{
				if(strcmp(shellmemory[victim_frame].value, "none") != 0)
					printf("%s\n", shellmemory[victim_frame].value);
			}
			
			printf("\nEnd of victim page contents.”");
			
			return victim_frame; // if no free frames, return the least recently used page
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
	char filename[100];
	strcpy(filename, "backing_store/");

	// get name of file from script path
	char *last_slash = strrchr(script, '/');
    if (last_slash) {
        strcat(filename, last_slash+1);
    } else {
		strcat(filename, script);
	}
	FILE *p = fopen(filename, "rt");

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
		add_node(frame_index); // add the frame to the tail of the LRU list
		if (frame_index == -1)
		{
			printf("No free frames in memory.\n");
			return;
		}

		// update page table
		int *page_add = pcb->page_table + i;
		*page_add = frame_index;

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


// Definition of a doubly linked list node
typedef struct Node
{
	int key;
	struct Node *prev;
	struct Node *next;
} Node;

// Head and tail of the doubly linked list
Node *head = NULL;
Node *tail = NULL;

// Hash table to quickly look up a page number and get a pointer to its corresponding node in the linked list
Node *hashtable[FRAME_STORE_SIZE / FRAME_SIZE];

// Remove a node from the linked list
void remove_node(Node *node)
{
	if (node->prev == NULL)
	{
		// Removing the head node
		head = node->next;
	}
	else
	{
		node->prev->next = node->next;
	}
	if (node->next == NULL)
	{
		// Removing the tail node
		tail = node->prev;
	}
	else
	{
		node->next->prev = node->prev;
	}
}

// Add a new node to the tail of the linked list
void add_node(int page)
{
	Node *node = (Node *)malloc(sizeof(Node));
	node->key = page;
	node->prev = tail;
	node->next = NULL;
	if (tail != NULL)
	{
		tail->next = node;
	}
	tail = node;
	if (head == NULL)
	{
		head = node;
	}
	// Update the hash table with a pointer to the new node
	hashtable[page] = node;
}

// Move an existing node to the tail of the linked list
void move_to_tail(Node *node)
{
	remove_node(node);
	node->prev = tail;
	node->next = NULL;
	tail->next = node;
	tail = node;
}

// Access a page by its page number
void access_page(int page)
{
	// Check if the page is already in the page frames
	Node *node = hashtable[page];
	if (node != NULL)
	{
		// If the page is already in the page frames, move its corresponding node to the tail of the linked list
		move_to_tail(node);
	}
	else
	{
		// If the page is not currently in the page frames, add a new node to the tail of the linked list
		add_node(page);
		// If the number of page frames is already at its maximum capacity, remove the head of the linked list (which represents the LRU page)
		if (head != NULL)
		{
			hashtable[head->key] = NULL;
			remove_node(head);
		}
	}
	// Update the hash table with a pointer to the node at the tail of the linked list (which represents the MRU page)
	hashtable[page] = tail;
}

// Get the least recently used page
int get_lru_page()
{
	if (head != NULL)
	{
		// If the head of the linked list is not null, return the key of the head node (which represents the LRU page)
		return head->key;
	}
	else
	{
		// If the linked list is empty, return -1 to indicate that there is no LRU page
		return -1;
	}
}