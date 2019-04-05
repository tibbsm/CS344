/********************************************************
*                                                       *
* FILENAME:     keygen.c                                *
* AUTHOR:       Marc Tibbs                              *
* EMAIL:        tibbsm@oregonstate.edu                  *
* CLASS:        CS344 (Winter 2019)                     *
* ASSIGNMENT:   Program 4 - OTP                         *
*                                                       *
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/********************************************************
*                                                       *
* FUNCTION NAME: keygen                                 *
*                                                       *
* DESCRIPTION: Prints a random key of the length of the *
*  passed in integer.                                   *
*                                                       *
* ARGUMENTS:                                            *
*   (1) keylength: The length of the key.               *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
********************************************************/
void keygen(int keylength) 
{
    int result, i;

    // Generate and print a random letter for 
    for (i = 0; i < keylength; i++)
    {
        result = 'A' + (rand() % 27);

        if (result == 'A' + 26)
        {
            result = ' ';
        }
        printf("%c", result);
    }
    printf("\n");
}


/********************************************************
*                                                       *
* FUNCTION NAME: checkArgs                              *
*                                                       *
* DESCRIPTION: Checks the number of arguments passed in.*
*  If not the right length then print error message and *
*  exit.                                                *
*                                                       *
* ARGUMENTS:                                            *
*   (1) args: The number of arguments.                  *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
********************************************************/
void checkArgs(int args)
{
    // If less than 2 arguments, exit and print to stderr.
    if (args < 2) 
    {
        fprintf(stderr, "ERROR: Too few arguments!\n");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    // Seed random generator
    srand(time(NULL));
    
    // Check arguments
    checkArgs(argc);

    // Generate a key of the length passed in
    keygen(atoi(argv[1]));

    return 0;
}