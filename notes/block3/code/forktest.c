#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void main()
{
	pid_t spawnpid = -5;
	int ten = 10;

	spawnpid = fork();
	switch (spawnpid)
	{
		// If something went wrong, fork() returns -1 to the
		// parent process and sets the global variable errno;
		// no child process was created
		case -1:
			perror("Hull Breach!");
			exit(1);
			break;
		// In the child process, fork() returns 0	
		case 0:
			ten = ten + 1;
			printf("I am the child! ten = %d\n", ten);
			break;
		// In the parent process, fork() returns the process
		// id of the child process that was just created
		default:
			ten = ten - 1;
			printf("I am the parent! ten = %d\n", ten);
			break;
	}
	printf("This will be executed by both of us!\n");
}
