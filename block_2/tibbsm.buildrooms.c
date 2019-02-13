/********************************************************
*							*
* FILENAME:		tibbsm.buildrooms.c		*
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
#include <unistd.h>
#include <fcntl.h>

/********************************************************
*							*
* STRUCTURE NAME: room	 			 	*
*  							*
* DESCRIPTION: Holds the room data, including its name, *
*	room type, number of connections & an array of  *
*	room 						*
*   names that it is connected to.			*
*							*
********************************************************/
struct room
{	
	int id;
	char* name;
	char* roomType;
	int numberOfOutboundConnections;
	struct room* outboundConnections[6];
};


/********************************************************
*							*
* FUNCTION NAME: is_graph_full()			*
*  							*
* DESCRIPTION: Checks if rooms have the minimum number 	*
*	of connections 				 	*
*							*
* ARGUMENTS: 						*
*	(1) rooms: route array 				*
*							*
* RETURNS: If true, returns 1. Else, returns 0		*
*							*
********************************************************/
int is_graph_full(struct room* rooms[])
{
	int i;

	for ( i = 0; i < 7; i++)
	{
		if (rooms[i]->numberOfOutboundConnections < 3)
		{
			return 0;
		}
	}
	return 1;
}

/********************************************************
*							*
* FUNCTION NAME: can_add_connection_from()		*
*  							*
* DESCRIPTION: Checks if another connection can be made *
*	to a room.	 			 	*
*							*
* ARGUMENTS: 						*
*	(1) room: a pointer to a room struct		*
*							*
* RETURNS: If true, returns 1. Else, returns 0		*
*							*
********************************************************/
int can_add_connection_from(struct room* room) 
{
	if (room->numberOfOutboundConnections < 6)
	{
		return 1;
	}

	return 0;
}

/********************************************************
*							*
* FUNCTION NAME: connection_already_exists()		*
*  							*
* DESCRIPTION: Checks if a room connection already 	*
* 	exists 						*
*							*
* ARGUMENTS: 						*
*	(1) roomOne: a pointer to a room struct		*
*	(2) roomTwo: a pointer to a room struct		*
*							*
* RETURNS: If true, returns 1. Else, returns 0		*
*							*
********************************************************/
int connection_already_exists(struct room* roomOne, struct room* roomTwo)
{
	int i;
	for ( i = 0; i < roomOne->numberOfOutboundConnections; i++ )
	{
		if ( roomOne->outboundConnections[i]->id == roomTwo->id )
			return 1;
	} 

	return 0;	
}

/********************************************************
*							*
* FUNCTION NAME: connect_rooms()			*
*  							*
* DESCRIPTION: Makes a connection between two rooms 	*
*							*
* ARGUMENTS: 						*
*	(1) roomOne: a pointer to a room struct		*
*	(2) roomTwo: a pointer to a room struct		*
*							*
* RETURNS: Nothing					*
*							*
********************************************************/
void connect_rooms(struct room* roomOne, struct room* roomTwo)
{
	// Add roomTwo to roomOne's connections
	int i = roomOne->numberOfOutboundConnections++;
	roomOne->outboundConnections[i] = roomTwo;

	// Add roomOne to roomTwo's connections
	int j = roomTwo->numberOfOutboundConnections++;
	roomTwo->outboundConnections[j] = roomOne;

	return;
}


/********************************************************
*							*
* FUNCTION NAME: add_random_connection()		*
*  							*
* DESCRIPTION: Adds a connection between two random 	* 
* 	rooms 						*
*							*
* ARGUMENTS: 						*
*	(1) rooms: an array of pointers to a room	* 
*		structs					*
*							*
* RETURNS: Nothing					*
*							*
********************************************************/
void add_random_connection(struct room* rooms[])
{
	int random_one, random_two;
	int flag = 1;

	srand((time(NULL)));
	
	while(flag == 1)
	{
		random_one = rand() % 7;

		if (can_add_connection_from(rooms[random_one]) == 1)
		{
			flag = 0;		
		}
	}

	do
	{
		random_two = rand() % 7;
	} 
	while (can_add_connection_from(rooms[random_two]) == 0 || 
			random_one == random_two || 
			connection_already_exists(rooms[random_one], 
				rooms[random_two]));

	connect_rooms(rooms[random_one], rooms[random_two]);	
}

/********************************************************
*							*
* FUNCTION NAME: initialize_rooms()			*
*  							*
* DESCRIPTION: Initializes room structs			*
*							*
* ARGUMENTS: 						*
*	(1) rooms: an array of pointers to a room	*
*		 structs				*
*							*
* RETURNS: Nothing					*
*							*
********************************************************/
void initialize_rooms(struct room* rooms[])
{
	// Initialize variables
	int index, random;
	int option[] = { 0,1,2,3,4,5,6,7,8,9 };

	// 10 potential room names to pick from
	char* RoomNames[] = { "alpha", "beta", "gamma", "delta", 
		"epsilon", "zeta", "eta", "theta", "iota", "kappa" };

	// Seed random number generator
	srand((time(NULL)));

	// For each room, generate a unique random number and
	// intialize that room with a random name 
	for ( index = 0; index < 7; index++)
	{
		do
		{
			random = rand() % 10;
		} 
		while(option[random] == -1);

		option[random] = -1;	

		rooms[index]->id = index;
		rooms[index]->numberOfOutboundConnections = 0;
		rooms[index]->name = RoomNames[random];
	}
}

/********************************************************
*							*
* FUNCTION NAME: give_rooms_types()			*
*  							*
* DESCRIPTION: Randomly assigns types to rooms 		*
*							*
* ARGUMENTS: 						*
*	(1) rooms: an array of pointers to a room	* 
*		structs					*
*							*
* RETURNS: Nothing					*
*							*
********************************************************/
void give_rooms_types(struct room* rooms[])
{
	// Initialize variables
	int index, randomOne, randomTwo;

	// Generate random number 
	srand(getpid());
	randomOne = rand() % 7;

	// Set the room types to mid rooms
	for (index = 0; index < 7; index++)
	{
		rooms[index]->roomType = "MID_ROOM";
	}

	// Set random room to start room
	rooms[randomOne]->roomType = "START_ROOM";

	// Get new, unique random number
	do
	{
		randomTwo = rand() % 7;
	} while(randomOne == randomTwo);

	// Set random room to end room
	rooms[randomTwo]->roomType = "END_ROOM";

	return;
}

/********************************************************
*							*
* FUNCTION NAME: create_room_directory()		*
*  							*
* DESCRIPTION: Creates directory to store room files	*
*							*
* ARGUMENTS: 						*
*	(1) rooms: an array of pointers to a room	* 
*		structs					*
*							*
* RETURNS: Nothing					*
*							*
********************************************************/
void create_room_directory(struct room* rooms[])
{
	// Initialize proces id variable
	int pid = getpid();
	
	// Set directoryName variable to tibbsm.rooms.<pid>
	char directoryName[20];
	memset(directoryName, '\0', 20);
	char* prefix = "tibbsm.rooms.";
	sprintf(directoryName, "%s%d", prefix, pid);

	// Create directory
	mkdir(directoryName, 0755);

	// Variable to hold room filepaths
	char filePath[100];

	int i, j, k;
	for ( i = 0; i < 7; i++)
	{
		// Initialize filePath
		memset(filePath, '\0', 20);

		// Set filePath to ./tibbsm.rooms.<pid>/<room name>_room
		sprintf(filePath, "./%s/%s_room", directoryName, rooms[i]->name);		

		// Initialize file stream using filePath
		FILE* file = fopen(filePath, "w");

		// If error opening file stream
		if (file == NULL)
		{
			fprintf(stderr, "Could not open %s\n", filePath);
			perror("Error in create_room_directory()");
			exit(1);
		}

		j = rooms[i]->numberOfOutboundConnections;

		// Print room name, connections, and room type to file
		fprintf(file, "ROOM NAME: %s\n", rooms[i]->name);
		for ( k = 0; k < j; k++)
			fprintf(file, "CONNECTION %d: %s\n", k + 1, rooms[i]->outboundConnections[k]->name);
		fprintf(file, "ROOM TYPE: %s\n", rooms[i]->roomType);

		// Close file
		fclose(file);
	}
	return;
}

int main() 
{
	// Initilaize room structs
	struct room RoomOne, RoomTwo, RoomThree, RoomFour, RoomFive,
		RoomSix, RoomSeven;

	// Initialize an array of the room structs
	struct room* rooms[] = { &RoomOne, &RoomTwo, &RoomThree, &RoomFour,
		&RoomFive, &RoomSix, &RoomSeven };

	// Randomly name rooms and initialize room data.
	initialize_rooms(rooms);

	// Randomly assign rooms room types
	give_rooms_types(rooms);

	// Make connections 
	while (is_graph_full(rooms) == 0)
	{
		add_random_connection(rooms);
	}

	// Create the directory for rooms
	create_room_directory(rooms);
}

