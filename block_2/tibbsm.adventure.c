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
	char* name;
	char* roomType;
	int numberOfOutboundConnections;
	char* outboundConnections[6];
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

void load_rooms(char name[], struct room* rooms[]){
	DIR *dir = opendir(name);
	struct dirent *ent;

	char* token;
	char readBuffer[256];
	FILE* file;

	if (dir != NULL) {
		// printf("Opened directory %s successfully\n", name);
		
		int num = 0;
		while ((ent = readdir(dir)) != NULL) {
			if ( strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				//printf("%s\n", ent->d_name);
				
				char i[100];
				memset(&i, '\0', sizeof(i));
				sprintf(i, "%s/%s", name, ent->d_name);		
				
				file = fopen(i, "r");
			
				if (file == NULL) {
					printf("Cannot open file %s\n", ent->d_name);		
					continue;
				}
				
				rooms[num]->numberOfOutboundConnections = 0;
				int shortcut = 0;

				while(fgets(readBuffer, sizeof(readBuffer), file)) {
					
					token = strtok(readBuffer, ":");			
					if (strcmp(token, "ROOM NAME") == 0) {
						token = strtok(NULL, " \n");	
						rooms[num]->name = calloc(16, sizeof(char));
						strcpy(rooms[num]->name, token);
						
						// printf("Room name is: %s\n", rooms[num]->name);
					} else if (strcmp(token, "ROOM TYPE") == 0) {
						token = strtok(NULL, " \n");
						rooms[num]->roomType = calloc(16, sizeof(char));
						strcpy(rooms[num]->roomType, token);			
						
						// printf("Room type is: %s\n", rooms[num]->roomType);
					} else {
						token = strtok(NULL, " \n");	
						rooms[num]->outboundConnections[shortcut] = calloc(16, sizeof(char));
						strcpy(rooms[num]->outboundConnections[shortcut], token);
								
						//printf("Room is connected to: %s\n", token);
						// printf("Room is connected to: %s\n", rooms[num]->outboundConnections[shortcut]);
						rooms[num]->numberOfOutboundConnections++;
						shortcut++;
					}
		
					
				}
				num++;
				fclose(file);
			}

		}
		closedir(dir);
	}
}

void print_rooms(struct room* rooms[])
{
	int i, j, k;
	for ( i = 0; i < 7; i++)
	{
		j = rooms[i]->numberOfOutboundConnections;

		printf("ROOM NAME: %s\n", rooms[i]->name);
		for ( k = 0; k < j; k++)
			printf("CONNECTION %d: %s\n", k + 1, rooms[i]->outboundConnections[k]);
		printf("ROOM TYPE: %s\n\n", rooms[i]->roomType);
	}
}

int main() 
{
	printf("--Start of main()--\n");
/////////////////////////////////////////////////////////////////////////////////////////////////////

	// Initilaize room structs
	struct room RoomOne, RoomTwo, RoomThree, RoomFour, RoomFive,
		RoomSix, RoomSeven;

	// Initialize an array of the room structs
	struct room* rooms[] = { &RoomOne, &RoomTwo, &RoomThree, &RoomFour,
		&RoomFive, &RoomSix, &RoomSeven };

	char name[] = "tibbsm.rooms.maxpidis2^22";

/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Get most recent directory--\n");
	get_most_recent_rooms(name);	
	printf("Room name: %s\n", name);

/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Load the rooms--\n");
	load_rooms(name, rooms);

/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Start maze--\n");
	
	char input[20];
	int i, j, k;
	for ( i = 0; i < 7; i++) {
		if (strcmp(rooms[i]->roomType, "START_ROOM") == 0) {
			printf("CURRENT LOCATION: %s\n", rooms[i]->name);
			j = rooms[i]->numberOfOutboundConnections;
			printf("POSSIBLE CONNECTIONS: ");
			for ( k = 0; k < j; k++) {
				if (k+1 != j) {
					printf("%s, ", rooms[i]->outboundConnections[k]);
				} else {
					printf("%s.\n", rooms[i]->outboundConnections[k]);
				}

			}
		}
	}
	printf("WHERE TO? >");
	fgets(input,20,stdin);
	// printf("%s\n", input);

	for ( i = 0; i < 7; i++) {
		printf("%s == %s (%d)\n", input, input, strcmp(input, input));
		if (strcmp(rooms[i]->name, input) == 0) {
			printf("CURRENT LOCATION: %s\n", rooms[i]->name);
			j = rooms[i]->numberOfOutboundConnections;
			printf("POSSIBLE CONNECTIONS: ");
			for ( k = 0; k < j; k++) {
				if (k+1 != j) {
					printf("%s, ", rooms[i]->outboundConnections[k]);
				} else {
					printf("%s.\n", rooms[i]->outboundConnections[k]);
				}

			}
		}
	}
	printf("WHERE TO? >");
	fgets(input,20,stdin);
	printf("%s\n", input);



/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n\n--Game Interface--\n");	
	printf("CURRENT LOCATION: %s\n", "XYZZY");
	printf("POSSIBLE CONNECTIONS: %s, %s, %s.\n", "PLOVER", "Dungeon", "twisty");
	printf("WHERE TO? >\n\n");

	print_rooms(rooms);

	return 0;
}

