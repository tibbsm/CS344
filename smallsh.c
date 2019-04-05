/********************************************************
*                                                       *
* FILENAME:             smallsh.c                       *
* AUTHOR:               Marc Tibbs                      *
* EMAIL:                tibbsm@oregonstate.edu          *
* CLASS:                CS344 (Winter 2019)             *
* ASSIGNMENT:           Program 3                       *
*                                                       *
********************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

// Global variable to track foreground mode
int foregroundMode = 0;

/********************************************************
*                                                       *
* FUNCTION NAME: replace_word                           *
*                                                       *
* DESCRIPTION: Searches through string and replaces old *
*  sub-strings with a new sub-string.                   *
*                                                       *
* ARGUMENTS:                                            *
*   (1) string: String to search                        *
*   (2) old: Sub-string to replace                      *
*   (3) new: Sub-string to replace with                 *
*                                                       *
* RETURNS: Pointer to new string                        *
*                                                       *
********************************************************/
char* replace_word(const char* string, const char* old, const char* new)
{
    char* result;
    int i, count = 0;
    int newLen = strlen(new);
    int oldLen = strlen(old);

    // For each letter in the string
    for (i = 0; string[i] != '\0'; i++)
    {
        // Count occurences of the old sub-string
        if (strstr(&string[i], old) == &string[i])
        {
            count++;
            i += oldLen - 1;
        }
    }

    // Allocate memory for the result string
    result = (char*) malloc(i + count * (newLen - oldLen) + 1);

    i = 0;
    // Copy over the old string to the result string while replacing 
    // the substrings
    while (*string)
    {
        if (strstr(string, old) == string)
        {
            strcpy(&result[i], new);
            i += newLen;
            string += oldLen;
        }
        else
        {
            result[i++] = *string++;
        }
    }
    result[i] = '\0';
    return result;
}


/********************************************************
*                                                       *
* FUNCTION NAME: foreground_toggle                      *
*                                                       *
* DESCRIPTION: Keeps track of SIGTSTP signals to toggle *
*  foreground mode.                                     *
*                                                       *
* ARGUMENTS: None                                       *
*                                                       *
* RETURNS: None                                         *
*                                                       *
********************************************************/
void foreground_toggle(int signo) 
{
    foregroundMode++;
}

int main(int argc, char const *argv[])
{
    // Initialize sigaction structs
    struct sigaction foreground_action, ignore_action, default_action;

    // Pointer to ignore signal and return signal to normal usage
    ignore_action.sa_handler = SIG_IGN;
    default_action.sa_handler = SIG_DFL;

    // Pointer to foreground toggle function, used to catch SIGSTP
    foreground_action.sa_handler = foreground_toggle;
    // Block/delay all signals arriving while this mask is in place. 
    sigfillset(&foreground_action.sa_mask);
    // Not using any flags
    foreground_action.sa_flags = 0;

    // Signal sets to block SIGTSTP signals from running processes
    sigset_t mask;
    sigset_t orig_mask;

    // Add SIGTSTP to mask signal set
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);

    int foregroundSwitch = 0;

    // User command variables
    char input[2048];
    char* args[512];
    int argNum;

    // Variables to keep track of child processes
    int children[512];
    int childExitMethod = -5;
    int status = 0;
    int stdoutFlag, stdinFlag;

    // PID Variables
    int pid = getpid();
    char pidStr[21];
    char* pidResult = NULL;
    char pidSymbol[] = "$$";
    sprintf(pidStr, "%d", pid);

    int i = 0;

    // Initialize child process array
    while (i < 512)
    {
        children[i] = -1; 
        i++;
    }


    // Main loop
    while(1)
    {
        // Ignore SIGINT(^C) & set SIGTSTP(^Z) to toggle foreground mode
        sigaction(SIGINT, &ignore_action, NULL);
        sigaction(SIGTSTP, &foreground_action, NULL);        

        // Initialize user input
        input[0] = '\0';
        args[0] = NULL;
        argNum = 0;
        

        i = 0;

        // Check for finished background processes and "deal with them"
        while (i < 512){
            pid_t actualPid = waitpid(children[i], &childExitMethod, WNOHANG);

            // Record child's exit status 
            if (childExitMethod > 0)
            {
                status = 1;
            }
            else 
            {
                status = 0;
            }  

            // For for child processes that have completed
            if (actualPid != 0 && children[i] != -1) 
            {
                // Print child process's pid and exit status or the signal that killed it
                if (WIFEXITED(childExitMethod))
                {
                    printf("background pid %d is done: exit value %d\n", children[i], WEXITSTATUS(childExitMethod));
                    fflush(stdout);
                }
                else
                {
                    printf("background pid %d is done: terminated by signal %d\n", children[i], WTERMSIG(childExitMethod));
                    fflush(stdout);
                }

                // Remove child process from array
                children[i] = -1;
            }
            i++;
        }

        // Check if foreground mode has been toggled
        if (foregroundSwitch != foregroundMode) 
        {
            // Account for multiple toggles
            foregroundMode = foregroundMode % 2;

            // Enter foreground mode
            if (foregroundMode == 1) {
                printf("\nEntering foreground-only mode (& is now ignored)\n");
                fflush(stdout);
                foregroundSwitch = 1;
                continue;
            }
            // Exit foreground mode
            else {
                printf("\nExiting foreground-only mode\n");
                fflush(stdout);
                foregroundSwitch = 0;
                continue;
            }
        }

        // Prompt 
        printf(": ");
        fflush(stdout);

        // Get user input
        fgets(input,2048,stdin);

        //Strip the new line off user input
        size_t ln = strlen(input)-1;
        if (input[ln] == '\n')
        {
            input[ln] = '\0';
        }

        i = 0;
        // Get first non-white space character
        while (input[i] == ' ')
        {
            i++;
        }

        // If line is blank or a comment, do nothing.
        if (input[i] == '#' || input[0] == '\0')
        {
            continue;
        }

        // Replace "$$" with PID
        pidResult = replace_word(input, pidSymbol, pidStr);

        // Tokenize string
        char* p = strtok(pidResult, " ");
        while (p != NULL)
        {
            args[argNum++] = p;
            p = strtok(NULL, " ");
        }
        args[argNum] = NULL;

/********************************************************
*   BUILT-IN FUNCTIONS                                  *
********************************************************/

        // Print exit status
        if (strcmp(args[0], "status") == 0)
        {
            printf("exit value %d\n", status);
            fflush(stdout);
        } 
        // Exit program
        else if (strcmp(args[0], "exit") == 0)
        {
            // Kill any remaining background processes
            i = 0;
            while (i < 512)
            {
                if (children[i] != -1)
                {
                    kill(children[i], SIGKILL);     
                }
                i++;
            }
            exit(0);
        }
        // Change directories
        else if (strcmp(args[0], "cd") == 0)
        {  
            if (argNum == 1) {
                chdir(getenv("HOME"));
            }
            else {
                chdir(args[1]);
            }
        }

/********************************************************
*   COMMAND EXECUTION                                   *
********************************************************/

        else 
        {            
            pid_t spawnPid = -5;
            childExitMethod = -5;
            int background = -5;

            // Check for the background argument
            if(strcmp(args[argNum-1], "&") == 0){
                // Flag process to be run in the background
                background = 1;
                args[argNum-1] = NULL;
            } 
            else 
            {
                // Flag process to be run in the foreground
                background = 0;
            }

            // Foreground mode is activated...
            if (foregroundSwitch == 1)
            {
                // Flag process to be run in the foreground
                background = 0;
            }
            
/********************************************************
*   FORK                                                *
********************************************************/

            spawnPid = fork();
    
            // Block SIGTSTOP during processes
            if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
                perror(NULL);
                return 1;
            }    

            if (spawnPid == -1)
            {
                perror(NULL);
                exit(1);
            }

/********************************************************
*   CHILD PROCESS                                       *
********************************************************/

            else if(spawnPid == 0)
            {

/********************************************************
*   CHECK FOR '<' & '>'                                 *
********************************************************/

                stdoutFlag = 1;
                stdinFlag = 1;
                i = 0;
                while(args[i] != NULL)
                {
                    if(strcmp(args[i],">") == 0){

                        stdoutFlag = 0;

                        // Open file to write to
                        int targetFD = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if(targetFD == -1){
                            perror(NULL);
                            exit(1);
                        }

                        // Redirect stdout to the opened file
                        int result = dup2(targetFD, 1);
                        if(result == -1){
                            perror(NULL);
                            exit(2);
                        }

                        // Remove '>' from the argument array
                        while (args[i+2] != NULL)
                        {
                            args[i] = args[i+2]; 
                            args[i+1] = args[i+2]; 
                            i++;   
                        }
                        args[i] = NULL;
                        args[i+1] = NULL;
                        i=i-2;
                    }   
                    i++;
                }

                i = 0;
                while(args[i] != NULL)
                {
                    if(strcmp(args[i], "<") == 0){

                        stdinFlag = 0;

                        // Open file to read
                        int sourceFD = open(args[i+1], O_RDONLY);
                        if(sourceFD == -1){
                            perror(NULL);
                            fflush(stdout);
                            exit(1);
                        }

                        // Redirect stdin to opened file
                        int result = dup2(sourceFD,0);
                        if(result == -1){
                            perror(NULL);
                            exit(2);
                        }

                        // Remove '<' from the argument array
                        while (args[i] != NULL)
                        {
                            args[i] = args[i + 1]; 
                            i++;   
                        }
                        args[i] = NULL;
                        i--;
                    }
                    i++;
                }

                // If marked as a foreground process
                if (background == 0)
                {
                    // Return SIGINT(^C) to it's default action
                    sigaction(SIGINT, &default_action, NULL);
                    
                    // Execute user command as new process
                    if(execvp(args[0], args) == -1){
                        perror(NULL);
                        fflush(stdout);
                        exit(2);
                    }
                }
                // If background process
                else 
                {        

                    if (stdoutFlag == 1)
                    {
                        // Redirect stdout to /dev/null
                        int targetFD = open("/dev/null", O_WRONLY);
                        if(targetFD == -1){
                            perror(NULL);
                            exit(1);
                        }
                        int result = dup2(targetFD, 1);
                        if(result == -1){
                            perror(NULL);
                            exit(2);
                        }
                    }

                    if (stdinFlag == 1)
                    {
                        // Redirect stdin to /dev/null
                        int sourceFD = open("/dev/null", O_RDONLY);
                        if(sourceFD == -1){
                            perror(NULL);
                            fflush(stdout);
                            exit(1);
                        }
                        int result = dup2(sourceFD,0);
                        if(result == -1){
                            perror(NULL);
                            exit(2);
                        }
                    }

                    // Execute user command as new process
                    if(execvp(args[0], args) == -1){
                        perror(NULL);
                        fflush(stdout);
                        exit(2);
                    }
                }
            }

/********************************************************
*   END OF CHILD PROCESS                                *
********************************************************/
        
            // Wait for foreground processes
            if (background == 0)
            {               
                waitpid(spawnPid, &childExitMethod, 0);

                // Set exit status
                if (childExitMethod > 0)
                {
                    status = 1;
                }
                else 
                {
                    status = 0;
                }

                // Print message if process ended by a signal
                if(WTERMSIG(childExitMethod))
                {
                    printf("terminated by signal %d\n", WTERMSIG(childExitMethod));
                    fflush(stdout);
                }
            } 

            // Record background process data
            else
            {
                // Print background pid
                printf("background pid is %d\n", spawnPid);
                fflush(stdout);

                i = 0;
                // Store process pid in array
                while (children[i] > -1)
                {
                    i++;
                }
                children[i] = spawnPid;
            }

            // Release SIGTSTOPs fired during process
            if (sigprocmask(SIG_SETMASK, &orig_mask, NULL) < 0) {
                perror(NULL);
                return 1;
            }
        }
    }
    return 0;
}
