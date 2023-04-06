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
    char command[1024];
    sprintf(command, "rm -rf %s", dirname);
    system(command);
}

int store_script(char *script)
{
    int namelen = strlen(script);
    char *dest = "backing_store/";
    int destlen = strlen(dest);
    // printf("namelen: %d, destlen: %d\n", namelen, destlen);
    char *command = (char *)calloc(1, 3 + namelen + 1 + destlen + 1);
    strncat(command, "cp ", 4);
    strncat(command, script, namelen);
    strncat(command, " ", 2);
    strncat(command, dest, destlen);
    // printf("command: %s\n", command);

    int errCode = system(command);
    free(command);

    // change all semi colons to new line characters

    // open the script file
    char filename[100];
    strcpy(filename, "backing_store/");

    // get name of file from script path
    char *last_slash = strrchr(script, '/');
    if (last_slash)
    {
        strcat(filename, last_slash + 1);
    }
    else
    {
        strcat(filename, script);
    }
    FILE *fp = fopen(filename, "r+"); // open for reading and writing
    // printf("Opening file: %s\n", filename);
    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == ';') {
            // printf("Found a semicolon\n");
            fseek(fp, -1, SEEK_CUR);    // Move the file pointer back one character
            fputc('\n', fp);            // Replace semicolon with newline
        }
    }

    fclose(fp);

    return errCode;
}

int get_number_lines(char *script)
{
    // count number of lines in file to find contiguous space in memory for it
    int lines = 0;
    char ch;

    char filename[100];
    strcpy(filename, "backing_store/");

    // get name of file from script path
    char *last_slash = strrchr(script, '/');
    if (last_slash)
    {
        strcat(filename, last_slash + 1);
    }
    else
    {
        strcat(filename, script);
    }

    FILE *p2 = fopen(filename, "rt");
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
