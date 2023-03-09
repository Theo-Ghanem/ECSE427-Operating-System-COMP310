#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "ready_queue.h"

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

// Start of everything
int main(int argc, char *argv[])
{
    printf("%s\n", "Shell version 1.2 Created January 2023");
    help();

    char prompt = '$';              // Shell prompt
    char userInput[MAX_USER_INPUT]; // user's input stored here
    int errorCode = 0;              // zero means no error, default

    // init user input
    for (int i = 0; i < MAX_USER_INPUT; i++)
        userInput[i] = '\0';

    // init shell memory
    mem_init();

    // init the ready queue [For Assignment 2]
    queue_init();

    while (1)
    {
        if (isatty(STDIN_FILENO)) // check if stdin is a terminal
        {
            printf("%c ", prompt); // display prompt only if not in batch mode
        }

        fgets(userInput, MAX_USER_INPUT - 1, stdin); // get user input
        if (strlen(userInput) > 0)
        {
            errorCode = parseInput(userInput);
            if (errorCode == -1)
                exit(99); // ignore all other errors
            memset(userInput, 0, sizeof(userInput));
        }
    }

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

        while (ui[a] != ';' && ui[a] != '\n' && ui[a] != '\0' && a < 1000)
        {
            for (b = 0; ui[a] != ';' && ui[a] != '\0' && ui[a] != '\n' && ui[a] != ' ' && a < 1000; a++, b++) // loop through the word
            {
                tmp[b] = ui[a];
                // extract a word
            }
            tmp[b] = '\0';          // terminate the word
            words[w] = strdup(tmp); // copy the word to the array of words
            w++;                    // increment wordID

            if (ui[a] == '\0') // if end of input
                break;
            if (ui[a] != ';') // should not increment for ';' since inner while loop needs to speperate one line commands!
                a++;
        }

        // for (int i = 0; i < w; i++) //debugging
        //     printf("word %d: %s  \n", i, words[i]); // debugging

        errorCode = interpreter(words, w); // send the word to the interpreter
        w = 0;
        memset(words, 0, sizeof(words)); // empty the array of words
        memset(tmp, 0, sizeof(tmp));     // empty the array of words

        if (errorCode == -1)
            return errorCode;
        if (ui[a] == '\0' || ui[a] == '\n') // if end of input
            break;
    }
    return errorCode;
}
