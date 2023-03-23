#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "paging.h"

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
