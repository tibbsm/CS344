#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
    pid_t spawnPid = -5;
    int childExitMethod = -5;

    spawnPid = fork();
    if (spawnPid == -1) 
    {
        perror("Hull Breach!\n");
        exit(1);
    }
    else if (spawnPid == 0)
    {
        printf("CHILD: PID: %d, exiting!\n", spawnPid);
        exit(0);
    }

    printf("PARENT: PID: %d, waiting...\n", spawnPid);
    waitpid(spawnPid, &childExitMethod, 0);
    printf("PARENT: Child process terminated, exiting!\n");
    exit(0);
}
