#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "shellmemory.h"
#include "shell.h"
#include "pcb.h"

int MAX_ARGS_SIZE = 7; // This was 3 initially, but changed to 7 for set function

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

int badcommandTooManyTokens()
{
	printf("%s\n", "Bad command: Too many tokens");
	return 1;
}

int badcommandMkdir()
{
	printf("%s\n", "Bad command: my_mkdir");
	return 1;
}

int badcommandCd()
{
	printf("%s\n", "Bad command: my_cd");
	return 1;
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

	if (args_size < 1)
	{
		return badcommand();
	}
	if (args_size > MAX_ARGS_SIZE)
	{
		return badcommandTooManyTokens();
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

	// NEW  set command:DONE
	// for 1.2.1 this needs to be changed to have a minimum of 3 (set VAR STRING) and maximum of 8 (STRING can be up to 5 tokens)
	else if (strcmp(command_args[0], "set") == 0)
	{
		if (args_size < 3)
			return badcommand();
		else if (args_size > 7)
		{
			return badcommandTooManyTokens();
		}
		char stringOfValues[1000];						   // intialize string to hold multiple tokens
		memset(stringOfValues, 0, sizeof(stringOfValues)); // empty the string
		for (int i = 2; i < args_size; i++)				   // start at 2 because 0 is set and 1 is the variable name
		{
			strcat(stringOfValues, command_args[i]); // add the token to the string
			if (i != args_size - 1)
			{
				strcat(stringOfValues, " "); // add a space to the string
			}
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
	
	// load code into memory
	errCode = mem_load_script(script);
	
	// create PCB
	

	return errCode;
}

// 1.2.2 Add the echo command
// Done
int echo(char *var)
{
	// if first character of string is not a $ then just print the string
	if (var[0] != '$')
	{
		printf("%s\n", var);
		return 0;
	}

	memmove(var, var + 1, strlen(var)); // remove the $ from the string

	if (strcmp(mem_get_value(var), "Variable does not exist") != 0) // check if var is in shell memory
	{
		printf("%s\n", mem_get_value(var));
		return 0;
	}
	else // if var is not in shell memory, print an empty line
	{
		printf("\n");
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

// helper function for sort
// compares char a and b
// returns -1 if a before b, 0 if a == b, 1 if b before a
int compareChar(char a, char b)
{
	// used cytpe.h library to determine if a char is a number, capital letter, or lowercase letter
	// instead of using ASCII values to reduce clutter

	if (isdigit(a) && isalpha(b))	   // if a is a number and b is a letter
		return -1;					   // a comes before b
	else if (isalpha(a) && isdigit(b)) // if a is a letter and b is a number
		return 1;					   // b comes before a
	else if (isalpha(a) && isalpha(b)) // when both are letters
	{
		if (tolower(a) == tolower(b)) // need logic to compare caps and min
		{
			if (isupper(a) && islower(b))
				return -1;
			else if (islower(a) && isupper(b))
				return 1;
			else
				return 0;
		}
		else // if not the same letter then use ASCII values
		{
			if (tolower(a) < tolower(b))
				return -1;
			else
				return 1;
		}
	}
	else
	{
		if (tolower(a) < tolower(b))
			return -1;
		else
			return 1;
	}
}

// helper function for my_ls
// sorts the files in alphabetical order (nums, then caps, then min)
int sort(const struct dirent **a, const struct dirent **b)
{
	const char *nameA = (*a)->d_name; // get the name of the first file
	const char *nameB = (*b)->d_name; // get the name of the second file

	// compare per char until a difference is found (i.e. not 0)
	for (int i = 0; i < strlen(nameA) && i < strlen(nameB); i++)
	{
		if (compareChar(nameA[i], nameB[i]) == 1) // if 1 then nameA is after nameB
			return 1;
		else if (compareChar(nameA[i], nameB[i]) == -1) // if -1 then nameA is before nameB
			return -1;
		// if 0 then continue to next char
	}
	return 0;
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
	// https://lloydrochester.com/post/c/list-directory/
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

	// check if dirname starts with a $
	if (dirName[0] == '$')
	{
		memmove(dirName, dirName + 1, strlen(dirName)); // remove the $ from the string

		// check if var is in shell memory
		if (strcmp(mem_get_value(dirName), "Variable does not exist") != 0) // check if dirName is in shell memory
		{
			char *space = strchr(mem_get_value(dirName), ' ');
			if (space != NULL)
			{
				return badcommandMkdir();
			}

			strcpy(dirName, mem_get_value(dirName)); // if it's in memory, copy the value to dirName
		}

		else
		{
			return badcommandMkdir();
		}
	}

		// inspired from stackoverflow post:
		// https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c
		struct stat st = {0}; // used to check if directory exists
		
		if (stat(dirName, &st) == -1) // if directory does not exist
		{
			mkdir(dirName, 0777); // 0777: Allows the owner, group, and others to read, write, and execute the directory.
		}
	return 0;
}

// 1.2.4 Add the my_touch filename command
// creates a new empty file inside the current directory. filename is an alphanumeric string
int my_touch(char *fileName)
{
	FILE *file = fopen(fileName, "w"); // create a new file with the name fileName, "w" means write only
	fclose(file);
	return 0;
}

// 1.2.4 add the my_cd command
// changes current directory to directory dirname, inside the current directory. If
// dirname does not exist inside the current directory, my_cd displays “Bad command: my_cd” and stays
// inside the current directory. dirname should be an alphanumeric string
int my_cd(char *dirName)
{
	int status = chdir(dirName); // attempt to change directory

	if (status != 0)
	{ // if directory does not exist
		return badcommandCd();
	}
	return 0;
}
