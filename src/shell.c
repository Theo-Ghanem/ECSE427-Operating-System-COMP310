#ifndef FRAME_STORE_SIZE
#define FRAME_STORE_SIZE 600
#define VAR_STORE_SIZE 500
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "ready_queue.h"
#include "scheduler.h"
#include "disk.h"

void *manager_thread(void *arg);

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[])
{
    // init shell memory
    mem_init();

    // init the ready queue [For Assignment 2]
    queue_init();

    // Setting  up  the  backing store for  paging [For Assignment 3]
    init_backing_store();

    // init the lru
    init_lru();

    // thread_pool_t pool;
    // init_thread_pool(&pool);

    printf("%s\n", "Shell version 1.2 Created January 2023\n");

    // Print out the frame store size and variable store size
    printf("Frame Store Size = %d; Variable Store Size = %d\n", FRAME_STORE_SIZE, VAR_STORE_SIZE);

    help();

    char prompt = '$';              // Shell prompt
    char userInput[MAX_USER_INPUT]; // user's input stored here
    int errorCode = 0;              // zero means no error, default

    // init user input
    for (int i = 0; i < MAX_USER_INPUT; i++)
        userInput[i] = '\0';

    while (1)
    {
        if (isatty(STDIN_FILENO)) // check if stdin is a terminal
        {
            printf("%c ", prompt); // display prompt only if not in batch mode
        }

        fgets(userInput, MAX_USER_INPUT - 1, stdin); // get user input
        if (feof(stdin))
        {
            freopen("/dev/tty", "r", stdin);
        }
        if (strlen(userInput) > 0)
        {
            errorCode = parseInput(userInput);

            if (errorCode == -1)
            {
                // if error, wait for all threads to complete then end
                // pthread_join(pool.threads[0], NULL);
                // pthread_join(pool.threads[1], NULL);
                exit(99);
            }

            memset(userInput, 0, sizeof(userInput));
        }
    }

    // wait for all threads to complete then end
    // pthread_join(pool.threads[0], NULL);
    // pthread_join(pool.threads[1], NULL);

    // clean up multi-threading
    // pthread_mutex_destroy(&pool.lock);
    // pthread_mutex_destroy(&pool.queue_lock);
    // pthread_cond_destroy(&pool.work_ready);

    return 0;
}

int parseInput(char ui[])
{
    char tmp[200];
    char *words[100];
    int a = 0;
    int b;
    int w = 0; // wordID
    int errorCode = 1;

    if (ui == NULL) // check if input is null (end of file)
    {
        freopen("/dev/tty", "r", stdin); // redirect stdin to terminal
        return 1;                        // return 1 to indicate end of file (not successful)
    }

    while (ui[a] != '\0' && a < 1000) // loop through the input
    {
        for (; (ui[a] == ';' || ui[a] == ' ') && a < 1000; a++)
            ; // skip white spaces
        for (int i = 0; i < 100; i++)
            words[i] = NULL;

        while (ui[a] != ';' && ui[a] != '\n' && ui[a] != '\0' && a < 1000)
        {
            for (b = 0; ui[a] != ';' && ui[a] != '\0' && ui[a] != '\n' && ui[a] != ' ' && a < 1000; a++, b++) // loop through the word
            {
                tmp[b] = ui[a];
                // extract a word
            }
            tmp[b] = '\0'; // terminate the word
            if (words[w] != NULL)
            {
                free(words[w]);
            }
            words[w] = strdup(tmp); // copy the word to the array of words
            w++;                    // increment wordID

            if (ui[a] == '\0') // if end of input
                break;
            if (ui[a] != ';') // should not increment for ';' since inner while loop needs to speperate one line commands!
                a++;
        }

        errorCode = interpreter(words, w); // send the word to the interpreter
        w = 0;
        // memset(words, 0, sizeof(words)); // empty the array of words
        // memset(tmp, 0, sizeof(tmp));     // empty the array of words

        if (errorCode == -1)
            return errorCode;
        if (ui[a] == '\0' || ui[a] == '\n') // if end of input
            break;
    }
    for (int i = 0; i < 100; i++)
    {
        if (words[i] != NULL)
            free(words[i]);
    }
    return errorCode;
}