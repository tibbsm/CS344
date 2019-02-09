/*
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
//#include <unistd.h>
//#include <fcntl.h>
#include <dirent.h>

struct room
{	
	int id;
	char* name;
	char* roomType;
	int numberOfOutboundConnections;
	struct room* outboundConnections[6];
};


/*********************************************************
*							 *
* FUNCTION NAME: create_room_directory()		 *
*  							 *
* DESCRIPTION: Creates the room directory		 * 
*  "tibbsm.rooms.<PID>" in current directory	 	 *
*							 *
* ARGUMENTS: None					 *
*							 *
* RETURNS: Nothing					 *
*							 *
*********************************************************/
int is_graph_full(struct room* rooms[])
{
	int i;

	for (int i = 0; i < 7; i++)
	{
		if (rooms[i]->numberOfOutboundConnections < 3)
			return 0;
	}
	return 1;
}

int main() 
{
	printf("Start of main()\n");

	// ptr for directory entry
	struct dirent *directoryEntry;

	// Pointer of DIR type
	DIR *directoryPtr = opendir(".");
	// 
	struct stat dStat;
	time_t latest = 0;

	if (directoryPtr == NULL)
	{
		printf("Could not open directory");
		return 0;	
	}

	while ((directoryEntry = readdir(directoryPtr)) != NULL)
	{
		printf("%d \n", directoryPtr->directoryEntry.d_name);		
	}

	close(directoryPtr);
	return 0;
	
}

