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
#include <fcntl.h>

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

void get_most_recent_rooms(char iname[]) 
{
	DIR *dir = opendir(".");
	struct dirent *ent;
	struct stat st;
	time_t time = 0;
	char name[] = "tibbsm.rooms.maxpidis2^22";

	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			memset(&st, 0, sizeof(st));
			if (stat(ent->d_name, &st) < 0) {
				printf("Error getting file info.\n");
				continue;
			}
			
			if ((st.st_mode & S_IFDIR) != S_IFDIR) {
				//printf("Not a directory \n");
				continue;
			}

			if (st.st_mtime > time 
				&& strcmp(ent->d_name, ".") != 0 
				&& strcmp(ent->d_name, "..") != 0) {
				//printf("%s\n", ent->d_name);
				strcpy(name, ent->d_name);
				//printf("%s\n", name);
				time = st.st_mtime;
			}
		}
		printf("File name: %s - Time: %d\n", name, (int) time);
		strcpy(iname, name);
		closedir(dir);
	} else {
                printf("Error: Couldn't open directory");
 	}
	return;
}

int main() 
{
	printf("--Start of main()--\n");
/////////////////////////////////////////////////////////////////////////////////////////////////////

	char name[] = "tibbsm.rooms.maxpidis2^22";

/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Get most recent directory--\n");
	get_most_recent_rooms(name);	
	printf("Room name: %s\n", name);

/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Load the rooms--\n");
	DIR *dir = opendir(name);
	struct dirent *ent;

	char readBuffer;
	FILE* file;

	if (dir != NULL) {
		printf("Opened directory %s successfully\n", name);
		
		while ((ent = readdir(dir)) != NULL) {
			if ( strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				printf("%s\n", ent->d_name);
				
				char i[100];
				memset(&i, '\0', sizeof(i));
				sprintf(i, "%s/%s", name, ent->d_name);		
				
				file = fopen(i, "r");
			
				if (file == NULL) {
					printf("Cannot open file %s\n", ent->d_name);		
					continue;
				}
						
				readBuffer = fgetc(file);
				while(readBuffer != EOF) {
					printf("%c", readBuffer);
					readBuffer = fgetc(file);
				}
			
				fclose(file);
			}

	

		}
		closedir(dir);
	}
	
/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Game Interface--\n");	
	printf("CURRENT LOCATION: %s\n", "XYZZY");
	printf("POSSIBLE CONNECTIONS: %s, %s, %s.\n", "PLOVER", "Dungeon", "twisty");
	printf("WHERE TO? >");

	return 0;
}

