#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void execute(char** argv)
{
    if(execvp(*argv, argv) < 0)
    {
        perror("Exec failure!");
        exit(1);
    }
}

void main()
{
    char* args[3] = {"ls", "-a", NULL};
    printf("Replaced process with: %s %s\n", args[0], args[1]);
    execute(args);
}
