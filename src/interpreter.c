#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 8; // This was 3 initially, but changed to 8 for set function

int badcommand()
{
	printf("%s\n", "Unknown Command");
	return 1;
}

// For run command only
int badcommandFileDoesNotExist()
{
	printf("%s\n", "Bad command: File not found");
	return 3;
}

int help();
int quit();
int set(char *var, char *value);
int print(char *var);
int echo(char *var);
int run(char *script);
int badcommandFileDoesNotExist();
int my_ls();
int my_mkdir(char *dirName);
int my_touch(char *fileName);
int my_cd(char *dirName);

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size)
{
	int i;

	if (args_size < 1 || args_size > MAX_ARGS_SIZE)
	{
		return badcommand();
	}

	for (i = 0; i < args_size; i++)
	{ // strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help") == 0)
	{
		// help
		if (args_size != 1)
			return badcommand();
		return help();
	}
	else if (strcmp(command_args[0], "quit") == 0)
	{
		// quit
		if (args_size != 1)
			return badcommand();
		return quit();
	}

	// OLD set command
	// else if (strcmp(command_args[0], "set")==0) {
	// 	//set
	// 	if (args_size != 3) return badcommand();
	// 	return set(command_args[1], command_args[2]);

	// NEW  set command:DONE
	// for 1.2.1 this needs to be changed to have a minimum of 3 (set VAR STRING) and maximum of 8 (STRING can be up to 5 tokens)
	else if (strcmp(command_args[0], "set") == 0)
	{
		if (args_size < 3)
			return badcommand();
		else if (args_size > 7)
		{
			printf("%s\n", "Bad Command: Too many tokens");
			return 1;
		}
		char stringOfValues[1000];						   // intialize string to hold multiple tokens
		memset(stringOfValues, 0, sizeof(stringOfValues)); // empty the string
		for (int i = 2; i < args_size; i++)
		{
			strcat(stringOfValues, command_args[i]);
			strcat(stringOfValues, " ");
		}

		return set(command_args[1], stringOfValues); // need to be able to take more arguments //changed
	}
	else if (strcmp(command_args[0], "print") == 0)
	{
		// print
		if (args_size != 2)
			return badcommand();
		return print(command_args[1]);
	}
	else if (strcmp(command_args[0], "run") == 0)
	{
		// run
		if (args_size != 2)
			return badcommand();
		return run(command_args[1]);
	}
	// for 1.2.2 add echo command DONE
	else if (strcmp(command_args[0], "echo") == 0)
	{
		// echo
		if (args_size != 2)
			return badcommand();
		return echo(command_args[1]);
	}

	// for 1.2.4 my_ls command
	else if (strcmp(command_args[0], "my_ls") == 0)
	{
		// my_ls
		if (args_size != 1)
			return badcommand();
		return my_ls();
	}

	// for 1.2.4 my_mkdir command
	else if (strcmp(command_args[0], "my_mkdir") == 0)
	{
		// my_mkdir
		if (args_size != 2)
			return badcommand();
		return my_mkdir(command_args[1]);
	}

	// for 1.2.4 my_touch command
	else if (strcmp(command_args[0], "my_touch") == 0)
	{
		// my_touch
		if (args_size != 2)
			return badcommand();
		return my_touch(command_args[1]);
	}

	// for 1.2.4 my_cd command
	else if (strcmp(command_args[0], "my_cd") == 0)
	{
		// my_cd
		if (args_size != 2)
			return badcommand();
		return my_cd(command_args[1]);
	}

	else
		return badcommand();
}

int help()
{

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%s\n", help_string);
	return 0;
}

int quit()
{
	printf("%s\n", "Bye!");
	exit(0);
}

// 1.2.1 Enhance the set command
// DONE
int set(char *var, char *value)
{
	char *link = "=";
	char buffer[1000];
	strcpy(buffer, var);
	strcat(buffer, link);
	strcat(buffer, value);

	mem_set_value(var, value);

	return 0;
}

int print(char *var)
{
	printf("%s\n", mem_get_value(var));
	return 0;
}

int run(char *script)
{
	int errCode = 0;
	char line[1000];
	FILE *p = fopen(script, "rt"); // the program is in a file

	if (p == NULL) // if the file does not exist
	{
		return badcommandFileDoesNotExist();
	}

	fgets(line, 999, p); // read the first line
	while (1)
	{
		errCode = parseInput(line);	   // which calls interpreter()
		memset(line, 0, sizeof(line)); // empty the string

		if (feof(p)) // checks if we are at the end of the file
		{
			// go back to interactive mode and wait for the next command
			break;
		}
		fgets(line, 999, p);
	}

	fclose(p);

	return errCode;
}

// 1.2.2 Add the echo command
// Done
int echo(char *var)
{
	// check if first character of string is not a $ then just print the string
	if (var[0] != '$')
	{
		printf("%s\n", var);
		return 0;
	}

	memmove(var, var + 1, strlen(var)); // remove the $ from the string

	// if first character of string is a $, check if var is in shell memory
	if (strcmp(mem_get_value(var), "Variable does not exist") == 0)
	{
		printf("\n");
		return 0;
	}
	else
	{
		printf("%s\n", mem_get_value(var));
		return 0;
	}
}

// helper function for my_ls
// filters out the hidden files
int filter(const struct dirent *name)
{
  if (name->d_name[0] == '.')
	return 0;
  else
	return 1;
}

// helper function for my_ls
// sorts the files in alphabetical order (nums, then caps, then min)
int sort(const struct dirent **a, const struct dirent **b)
{
	return strcmp((*a)->d_name, (*b)->d_name);
}

// 1.2.4 Add the ls command
// lists all the files present in the current directory
int my_ls()
{
	// use dirent.h library to open and read the current directory

	// create a pointer to an array of pointers to store files in the current directory
	struct dirent **files;

	// scan directory, filter out hidden files, and sort the files (see helper functions above)
	// returns the number of files in the current directory
	int numFiles = scandir(".", &files, filter, sort); 

	// since already sorted, print files in array order
	for (int i = 0; i < numFiles; i++)
	{
		printf("%s\n", files[i]->d_name);
	}

	return 0;
}

// 1.2.4 Add the my_mkdir command
// creates a new directory with the name dirname in the current directory.
int my_mkdir(char *dirName)
{
}

// 1.2.4 Add the my_touch filename command
// creates a new empty file inside the current directory. filename is an alphanumeric string
int my_touch(char *fileName)
{
}

// 1.2.4 add the my_cd command
// changes current directory to directory dirname, inside the current directory. If
// dirname does not exist inside the current directory, my_cd displays “Bad command: my_cd” and stays
// inside the current directory. dirname should be an alphanumeric string
int my_cd(char *dirName)
{
}
