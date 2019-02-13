/********************************************************
*							*
* FILENAME:		tibbsm.adventure.c		*
* AUTHOR: 		Marc Tibbs 			*
* EMAIL:		tibbsm@oregonstate.edu		*
* CLASS: 		CS344 (Winter 2019)		*
* ASSIGNMENT:		Program 2			*
*							*
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h> 

// Global mutex variable 
pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;

/********************************************************
*							*
* STRUCTURE NAME: room	 			 	*
*  							*
* DESCRIPTION: Holds the room data, including its name, *
*	room type, number of connections & an array of  *
*	room names that it is connected to.		*
*							*
********************************************************/
struct room
{	
	char* name;
	char* roomType;
	int numberOfOutboundConnections;
	char* outboundConnections[6];
};

/********************************************************
*							*
* FUNCTION NAME: get_most_recent_rooms()		*
*  							*
* DESCRIPTION: Set the passed in char pointer to the 	*
*  name most recently touched directory in the current  *
*  directory.					 	*
*							*
* ARGUMENTS: (1) dirName: A pointer to the char array to*
*  hold the name of the most recently touched directory *
*							*
* RETURNS: Nothing					*
*							*
********************************************************/
void get_most_recent_rooms(char* dirName) 
{
	// Initialize directory variable to point to the current
	// directory's stream
	DIR *dir = opendir(".");
	
	// Initialize dirent variable
	struct dirent* ent;
	
	// Initialize stat variable
	struct stat st;
	
	// Initialize time variable to 0
	time_t time = 0;

	// Check if there was any issue accessing the current directory
	if (dir != NULL) 
	{
		// Check for contents in directory
		while ((ent = readdir(dir)) != NULL) 
		{
			// Set stat variable to null
			memset(&st, '\0', sizeof(st));

			// There's an issue accessing the file info, go to the next
			if (stat(ent->d_name, &st) < 0) 
			{
				continue;
			}
			
			// It's not a directory, go to the next
			if ((st.st_mode & S_IFDIR) != S_IFDIR) 
			{
				continue;
			}

			// Check for the latest accessed file/directory that is
			// not the current/previous directory
			if 	(st.st_mtime > time && 
				 strcmp(ent->d_name, ".") != 0 && 
				 strcmp(ent->d_name, "..") != 0) 
			{
				
				// Copy the directory name to dirName
				strcpy(dirName, ent->d_name);
				
				// Set time to the last time the directory was touched.
				time = st.st_mtime;
			}
		}

		// Close directory
		closedir(dir);

	} 
	else 
	{

		// There was an error accessing the current directory. 
        printf("Error: Couldn't open directory");

 	}
}


/********************************************************
*							*
* FUNCTION NAME: load_rooms()		 		*
*  							*
* DESCRIPTION: Set the passed in char pointer to the 	*
*  name most recently touched directory in the current  *
*  directory.				 	 	*
*							*
* ARGUMENTS: 						*
* 	(1) dirName: A char array holding the name of   *
* 		the room directory to access. 		*
* 	(2) rooms: An array of pointers to rooms structs*
*							*
* RETURNS: Nothing					*
*							*
********************************************************/
void load_rooms(char dirName[], struct room* rooms[]){

	// A pointer to the room directory stream
	DIR *roomDir = opendir(dirName);

	// A dirent variable
	struct dirent *ent;

	// A char pointer used to help read the room files
	char* token;

	// A char array used to store lines from the room files
	char readBuffer[256];

	// A FILE type to access the room files
	FILE* roomFile;

	// Int variable to hold index of room in rooms[] 
	int roomIndex = 0;

	// Int variable to index a room's connections
	int connectionIndex;

	// Check if the room directory is accessible
	if (roomDir != NULL) {

		// Check if room file is accessible
		while ((ent = readdir(roomDir)) != NULL) {
		
			// Ignore the current/previous directories
			if ( strcmp(ent->d_name, ".") != 0 && 
				 strcmp(ent->d_name, "..") != 0) 
			{

				// Read the room file
				char filePath[100];
				memset(&filePath, '\0', sizeof(filePath));
				sprintf(filePath, "%s/%s", dirName, ent->d_name);		
				roomFile = fopen(filePath, "r");
			
				// Go to next file if current file is not accessible
				if (roomFile == NULL) 
				{
					continue;
				}
				
				// Initialize current room's number of connections
				rooms[roomIndex]->numberOfOutboundConnections = 0;
				connectionIndex = 0;

				// Read each line of the room file into readBuffer
				while(fgets(readBuffer, sizeof(readBuffer), roomFile)) 
				{
					// Set token to contents of line before the colon
					token = strtok(readBuffer, ":");			

					// If line holds the room name, set the room's name 
					if (strcmp(token, "ROOM NAME") == 0) 
					{
						token = strtok( NULL, " \n");	
						rooms[roomIndex]->name = 
							calloc(16, sizeof(char));
						strcpy(rooms[roomIndex]->name, token);
					} 
					// If line holds the room type, set the room's type
					else if (strcmp(token, "ROOM TYPE") == 0) 
					{
						token = strtok( NULL, " \n");
						rooms[roomIndex]->roomType = 
							calloc(16, sizeof(char));
						strcpy(rooms[roomIndex]->roomType, token);			
					} 
					// Otherwise, set the room's connections
					else 
					{
						token = strtok(NULL, " \n");	
						  
						rooms[roomIndex]->
							outboundConnections[connectionIndex] = 
								calloc(16, sizeof(char));

						strcpy(rooms[roomIndex]->
							outboundConnections[connectionIndex],token);
							
						rooms[roomIndex]->numberOfOutboundConnections++;
 
						connectionIndex++;
					}
				} 
				// Increment room index and close current room file
				roomIndex++;
				fclose(roomFile);
			}

		}
		// Close the room directory
		closedir(roomDir);
	}
}

/********************************************************
*							*
* FUNCTION NAME: get_start_room()			*
*  							*
* DESCRIPTION: Get return the pointer to the maze's 	*
*  startroom.						*
*							*
* ARGUMENTS: 						*
* 	(1) rooms: An array of pointers to rooms structs*
*							*
* RETURNS: A pointer to the start room. Null if not 	*
* 	found.						*
*							*
********************************************************/
struct room* get_start_room(struct room* rooms[]) 
{
	int i;

	// Iterate thru rooms[] and find the start room
	for ( i = 0; i < 7; i++) {
		if (strcmp(rooms[i]->roomType, "START_ROOM") == 0) {
			return rooms[i];		
		}
	}

	// Return null if start room not found
	return NULL;
}

/********************************************************
*							*
* FUNCTION NAME: get_time()		 		*
*  							*
* DESCRIPTION: Writes the current date/time to file.    *
*							*
* ARGUMENTS: None 					*
*							*
* RETURNS: Nothing.					*
*							*
********************************************************/
void* get_time() 
{ 
	// Lock mutex 
	pthread_mutex_lock(&MUTEX);

	// Open file to hold the date/time
	FILE* file = fopen("currentTime.txt",  "w");

	char dateString[200];
    time_t timeStamp;
    struct tm *localTime;

	timeStamp = time(NULL);
    localTime = localtime(&timeStamp);

    if (localTime == NULL) 
    {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    // Get the date/time string
   if (strftime(dateString, sizeof(dateString), 
   					"%l:%M%p, %A, %B %e, %Y", localTime) == 0) 
	{
        fprintf(stderr, "strftime returned 0");
    }

	// Print date/time to file and close the file
	fprintf(file, "%s", dateString);
	fclose(file);

	// Unlock mutex
	pthread_mutex_unlock(&MUTEX);
	
    return 0; 
}


/********************************************************
*							*
* FUNCTION NAME: free_memory()		 		*
*  							*
* DESCRIPTION: Frees memory allocated to room structs   *
*							*
* ARGUMENTS: 						*
*	(1) rooms: Array of pointers to room structs	*
*							*
* RETURNS: Nothing.					*
*							*
********************************************************/
void free_memory(struct room* rooms[], pthread_t timeThread) 
{
	int i, j, k;
	for ( i = 0; i < 7; i++)
	{
		j = rooms[i]->numberOfOutboundConnections;

		free(rooms[i]->name);
		
		free(rooms[i]->roomType);

		for ( k = 0; k < j; k++)
		{
			free(rooms[i]->outboundConnections[k]);
		}	
	}

	pthread_mutex_unlock(&MUTEX);
	pthread_join(timeThread, NULL);  
	pthread_mutex_lock(&MUTEX);
}

/********************************************************
*							*
* FUNCTION NAME:  print_current_room()		 	*
*  							*
* DESCRIPTION: Prints the room's name and connections   *
*							*
* ARGUMENTS: 						*
*	(1) currentRoom: Pointers to room struct 	*
*							*
* RETURNS: Nothing.					*
*							*
********************************************************/
void print_current_room(struct room* currentRoom)
{
	int numOfConnections, connctIdx;

	// Print the current room's name
	printf("CURRENT LOCATION: %s\n", currentRoom->name);
	
	numOfConnections = currentRoom->numberOfOutboundConnections;
	
	// Print the current room's connections
	printf("POSSIBLE CONNECTIONS: ");
	for ( connctIdx = 0; connctIdx < numOfConnections; connctIdx++) {
		if (connctIdx+1 != numOfConnections) 
		{
			printf("%s, ", currentRoom->outboundConnections[connctIdx]);
		} 
		else 
		{
			printf("%s.", currentRoom->outboundConnections[connctIdx]);
		}
	}	
}

/********************************************************
*							*
* FUNCTION NAME:  get_user_input()			*
*  							*
* DESCRIPTION: Gets and validates user input   		*
*							*
* ARGUMENTS: 						*
*	(1) currentRoom: Pointers to room struct 	*
*	(2) timeThread: Thread to run get_time() 	*
*	(3) input: Char pointer with user input 	*
*							*
* RETURNS: Nothing.					*
*							*
********************************************************/
void get_user_input(struct room* currentRoom, pthread_t timeThread, 
						char* input) 
{
	// Initialize the input flag to false
	int inputFlag = 0;

	// Int variables to track room data
	int roomIndex, numOfConnections, connectionIndex;

	numOfConnections = currentRoom->numberOfOutboundConnections;

	// While the input flag is false
	while(inputFlag == 0) 
	{

		// Get the user's input and remove the new line
		printf("\nWHERE TO? >");
		fgets(input,20,stdin);

		printf("\n");
		size_t ln = strlen(input)-1;
		if (input[ln] == '\n')
		{
	    	input[ln] = '\0';
		}
	    
	    // Check if the input matches the possible room connections
	    for ( connectionIndex = 0; connectionIndex < numOfConnections; 
	    		connectionIndex++) 
	    {
			if (strcmp(currentRoom->
					outboundConnections[connectionIndex], input) == 0) 
			{
				// Set flag to true if it's a valid room
				inputFlag = 1;
			} 
		}

		// If user input is time
		if (strcmp(input, "time") == 0) 
		{
			// Unlock mutex to allow get_time() to run
			pthread_mutex_unlock(&MUTEX);

			// Destroy timeThread, lock mutex, and recreate the 
			// timeThread
			pthread_join(timeThread, NULL);  
			pthread_mutex_lock(&MUTEX);
			pthread_create(&timeThread, NULL, get_time, NULL);

			// Read the date/time from file
			char  timeBuffer;
			FILE* timeFile = fopen("currentTime.txt","r");

			// Read the contents from the time file
			while( (timeBuffer = fgetc(timeFile)) != EOF) {
				printf("%c", timeBuffer);
			}

			// Close the time file
			fclose(timeFile );
			printf("\n"); 	
		} 
		// Print error message if input not recognized
		else if(inputFlag == 0) 
		{
			printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
		}

	}
}

/********************************************************
*							*
* FUNCTION NAME:  go_to_room()			 	*	
*  							*
* DESCRIPTION: Changes current room to user's choice	*
*							*
* ARGUMENTS: 						*
*	(1) route: pointer to route array 		*
*	(2) steps: pointer to step count variable 	*
*	(3) input: pointer to user's input	 	*
*	(4) rooms: pointer to rooms array		*
*	(5) room: pointer to pointer to currentRoom	*
*							*
* RETURNS: Nothing.					*
*							*
********************************************************/
void go_to_room(int* route, int* steps, char* input, 
					struct room* rooms[], struct room** currentRoom)
{
	int roomIndex;
	for ( roomIndex = 0; roomIndex < 7; roomIndex++) {

			if (strcmp(rooms[roomIndex]->name, input) == 0) {
				*currentRoom = rooms[roomIndex];	
				route[*steps] = roomIndex;
				(*steps)++;	
			}
		}
}

/********************************************************
*							*
* FUNCTION NAME: print_end_game()			*	
*  							*
* DESCRIPTION: Prints end of game message, the user's 	* 
*	step count, and the user's route		*
*							*
* ARGUMENTS: 						*
*	(1) route: route array 				*
*	(2) steps: step count variable 			*
*	(4) rooms: pointer to rooms array		*
*							*
* RETURNS: Nothing.					*
*							*
********************************************************/
void print_end_game(int route[], int steps, struct room* rooms[])
{
	int roomIndex;

	// Print end of game message.
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
	for ( roomIndex = 0; roomIndex < 100; roomIndex++)
	{
		if (route[roomIndex] == -1)
		{
			break;
		}
		else
		{
			printf("%s\n", rooms[route[roomIndex]]->name);
		}

	}
}

int main() 
{
	// Initilaize room structs
	struct room RoomOne, RoomTwo, RoomThree, RoomFour, RoomFive,
		RoomSix, RoomSeven;

	// Initialize an array of the room structs
	struct room* rooms[] = { &RoomOne, &RoomTwo, &RoomThree, &RoomFour,
		&RoomFive, &RoomSix, &RoomSeven };

	// Char arrays to hold name of the rooms directory & user input
	char roomDir[256], input[20];

	// Int array to record the user's route
	int route[100] = {[0 ... 99] = -1};

	// Int to hold the number of steps the user makes in the game
	int steps = 0;

	// Thread to run the get_time function
	pthread_t timeThread;

	// Lock mutex and create the timeThread to run get_time()
	pthread_mutex_lock(&MUTEX);
	pthread_create(&timeThread, NULL, get_time, NULL);

	// Set roomDir to the most recently touched room directory
	get_most_recent_rooms(roomDir);	

	// Load the room data to the room structs
	load_rooms(roomDir, rooms);

	// Set the current room to the start room 
	struct room* currentRoom = get_start_room(rooms);

	// Play game until the end room is found
	while (strcmp(currentRoom->roomType, "END_ROOM") != 0) {

		// Print current room's info
		print_current_room(currentRoom);
		
		// Get user's input
		get_user_input(currentRoom, timeThread, input);

		// Set current room to the room the user chose.
		go_to_room(route, &steps, input, rooms, &currentRoom);
	}

	// Print out the end of game message
	print_end_game(route, steps, rooms);

	// Free memory to avoid memory leaks
	free_memory(rooms, timeThread);

	return 0;
}

