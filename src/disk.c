#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "disk.h"

int get_number_lines(char *script);

void init_backing_store()
{
    char *dirname = "backing_store";

    // Check if directory exists, and if so, delete it
    delete_backing_store();

    // Create the directory
    mkdir(dirname, 0700);
}

void delete_backing_store()
{
    char *dirname = "backing_store";

    // Check if directory exists, and if so, delete it
    struct stat st = {0};
    if (stat(dirname, &st) != -1)
    {
        char *argv[] = {"rm", "-rf", dirname, NULL};
        execv("/bin/rm", argv);
    }
}

int store_script(char *script)
{
    int namelen = strlen(script);
    char *dest = "backing_store/";
    int destlen = strlen(dest);
    char *command = (char *)calloc(1, 3 + namelen + 1 + destlen);
    strncat(command, "cp ", 4);
    strncat(command, script, namelen);
    strncat(command, " ", 2);
    strncat(command, dest, destlen);
    int errCode = system(command);
    free(command);
    return errCode;
}

int get_number_lines(char *script)
{
    // count number of lines in file to find contiguous space in memory for it
    int lines = 0;
    char ch;

    FILE *p2 = fopen(script, "rt");
    while (!feof(p2))
    {
        ch = fgetc(p2);
        if (ch == '\n')
        {
            lines++;
        }
    }
    fclose(p2);
    lines++; // add one for the last line (EOF)
    return lines;
}
