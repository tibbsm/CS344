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
#include <pthread.h> 


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
				// printf("%s\n", ent->d_name);
				
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

struct room * get_start_room(struct room* rooms[]) {
	int i;
	for ( i = 0; i < 7; i++) {
		if (strcmp(rooms[i]->roomType, "START_ROOM") == 0) {
			return rooms[i];		
		}
	}
	return NULL;
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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *get_time(void *vargp) 
{ 
	pthread_mutex_lock(&mutex);

	char outstr[200];
    time_t t;
    struct tm *tmp;

	t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

   if (strftime(outstr, sizeof(outstr), "%l:%M%p, %A, %B %e, %Y", tmp) == 0) {
        fprintf(stderr, "strftime returned 0");
    }

	// printf("Result string is %s\n", outstr);
	FILE* file = fopen("currentTime.txt",  "w");
	fprintf(file, "%s", outstr);
	pthread_mutex_unlock(&mutex);
	fclose(file);
    return 0; 
}

void free_memory(struct room* rooms[]) {
	int i, j, k;
	for ( i = 0; i < 7; i++)
	{
		j = rooms[i]->numberOfOutboundConnections;

		free(rooms[i]->name);
		free(rooms[i]->roomType);
		for ( k = 0; k < j; k++)
			free(rooms[i]->outboundConnections[k]);
		
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

	pthread_t thread; 

	pthread_mutex_lock(&mutex);
	pthread_create(&thread, NULL, get_time, NULL);

/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Get most recent directory--\n");
	get_most_recent_rooms(name);	
	printf("Room name: %s\n", name);

/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Load the rooms--\n");
	load_rooms(name, rooms);

/////////////////////////////////////////////////////////////////////////////////////////////////////

	printf("\n--Get Start Room--\n");
	struct room* currentRoom = get_start_room(rooms);

	printf("\n--Start Maze--\n");
	char input[20];
	int route[100] = {[0 ... 99] = -1};
	int i, j, k;
	int steps = 0;
// TODO: Loop here
	while (strcmp(currentRoom->roomType, "END_ROOM") != 0) {
		printf("CURRENT LOCATION: %s\n", currentRoom->name);
		j = currentRoom->numberOfOutboundConnections;
		printf("POSSIBLE CONNECTIONS: ");
		for ( k = 0; k < j; k++) {
			if (k+1 != j) {
				printf("%s, ", currentRoom->outboundConnections[k]);
			} else {
				printf("%s.", currentRoom->outboundConnections[k]);
			}
		}

		int flag = 0;
		while(flag == 0) {
			printf("\nWHERE TO? >");
			fgets(input,20,stdin);
			printf("\n");
			size_t ln = strlen(input)-1;
			if (input[ln] == '\n')
		    	input[ln] = '\0';
		    
		    for ( k = 0; k < j; k++) {
				if (strcmp(currentRoom->outboundConnections[k], input) == 0) {
					flag = 1;
				} 
			}

			if (strcmp(input, "time") == 0) {
				pthread_mutex_unlock(&mutex);
				pthread_join(thread, NULL);  
				pthread_mutex_lock(&mutex);
				pthread_create(&thread, NULL, get_time, NULL);

				char s;
				FILE* f=fopen("currentTime.txt","r");
				while((s=fgetc(f))!=EOF) {
					printf("%c",s);
				}
				fclose(f);
				printf("\n");
			} else if(flag == 0) {
				printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			}
		}

		for ( i = 0; i < 7; i++) {
			if (strcmp(rooms[i]->name, input) == 0) {
				currentRoom = rooms[i];	
				route[steps] = i;
				steps++;	
			}
		}
	}


	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
	for ( i = 0; i < 100; i++){
		// printf("%d\n", route[i]);
		if (route[i] == -1)
			break;
		else
			printf("%s\n", rooms[route[i]]->name);

	}

	pthread_mutex_unlock(&mutex);
	pthread_join(thread, NULL);  
	pthread_mutex_lock(&mutex);
	free_memory(rooms);


/////////////////////////////////////////////////////////////////////////////////////////////////////

	return 0;
}

