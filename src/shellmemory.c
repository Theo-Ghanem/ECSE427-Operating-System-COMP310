#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

struct memory_struct
{
	char *var;
	char *value;
};

struct memory_struct shellmemory[1000];

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

	int i;

	for (i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, var_in) == 0)
		{
			shellmemory[i].value = strdup(value_in);
			return;
		}
	}

	// Value does not exist, need to find a free spot.
	for (i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, "none") == 0)
		{
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		}
	}

	return;
}

// get value based on input key
char *mem_get_value(char *var_in)
{
	int i;

	for (i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, var_in) == 0)
		{

			return strdup(shellmemory[i].value);
		}
	}
	return "Variable does not exist";
}

// A2 1.2.1 Code loading I think
int mem_load_script(char *script, int *memLocation, int *memSize)
{

	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script, "rt"); // the program is in a file

	if (p == NULL) // if the file does not exist
	{
		printf("File does not exist\n");
		return 1;
	}

	// count number of lines in file to find contiguous space in memory for it
	int lines = 0;
	char ch;

	while (!feof(p))
	{
		ch = fgetc(p);
		if (ch == '\n')
		{
			lines++;
		}
	}

	// find contiguous space in memory for the file
	int startLine = 0;
	int contiguousFreeLines = 0;
	for (int i = 0; i < 1000 && contiguousFreeLines <= lines; i++)
	{
		if (strcmp(shellmemory[i].var, "none") == 0) // if the line is free
		{
			contiguousFreeLines++;
		}
		else
		{
			contiguousFreeLines = 0;
			startLine = i;
		}
	}

	// save lines into memory contiguously
	fgets(line, 999, p); // read the first line

	for (int currentLine = startLine; currentLine < startLine + lines; currentLine++)
	{
		// naming: filename_lineNumber
		int index = currentLine - startLine;
		char ind[20];
		sprintf(ind, "%d", index);
		char *name = strcat(script, strcat("_", ind));
		printf("mem: %s\n", name);
		shellmemory[currentLine].var = strdup(name);
		shellmemory[currentLine].value = strdup(line);

		memset(line, 0, sizeof(line)); // empty the string
		memset(line, 0, sizeof(name));

		fgets(line, 999, p);
	}

	fclose(p);

	*memLocation = startLine;
	*memSize = lines;

	sleep(5);

	return errCode;
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