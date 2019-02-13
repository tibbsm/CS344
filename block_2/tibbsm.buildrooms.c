/********************************************************
*														*
* FILENAME:		tibbsm.buildrooms.c						*
* AUTHOR: 		Marc Tibbs 								*
* EMAIL:		tibbsm@oregonstate.edu					*
* CLASS: 		CS344 (Winter 2019)						*
* ASSIGNMENT:	Program 2								*
*							 							*
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/********************************************************
*							 							*
* STRUCTURE NAME: room	 							 	*
*  							 							*
* DESCRIPTION: Holds the room data, including its name, *
*	room type, number of connections & an array of room *
*   names that it is connected to.						*
*														*
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
*							 							*
* FUNCTION NAME: is_graph_full()				 		*
*  										 				*
* DESCRIPTION: Checks if rooms have the minimum number 	*
*	of connections 								 	 	*
*							 							*
* ARGUMENTS: 			*
*	(1) rooms: route array 								*
*							 							*
* RETURNS: If true, returns 1. Else, returns 0			*
*							 							*
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
*							 							*
* FUNCTION NAME: can_add_connection_from()				*
*  										 				*
* DESCRIPTION: Checks if another connection can be made *
*	to a room.	 								 	 	*
*							 							*
* ARGUMENTS: 											*
*	(1) room: a pointer to a room struct				*
*							 							*
* RETURNS: If true, returns 1. Else, returns 0			*
*							 							*
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
*							 							*
* FUNCTION NAME: connection_already_exists()			*
*  										 				*
* DESCRIPTION: Checks if a room connection already 		*
* 	exists 												*
*							 							*
* ARGUMENTS: 											*
*	(1) roomOne: a pointer to a room struct				*
*	(2) roomTwo: a pointer to a room struct				*
*							 							*
* RETURNS: If true, returns 1. Else, returns 0			*
*							 							*
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
*							 							*
* FUNCTION NAME: connect_rooms()						*
*  										 				*
* DESCRIPTION: Makes a connection between two rooms 	*
*							 							*
* ARGUMENTS: 											*
*	(1) roomOne: a pointer to a room struct				*
*	(2) roomTwo: a pointer to a room struct				*
*							 							*
* RETURNS: Nothing										*
*							 							*
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
*							 							*
* FUNCTION NAME: add_random_connection()				*
*  										 				*
* DESCRIPTION: Adds a connection between two random 	* 
* 	rooms 												*
*							 							*
* ARGUMENTS: 											*
*	(1) rooms: an array of pointers to a room structs	*
*							 							*
* RETURNS: Nothing										*
*							 							*
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
*							 							*
* FUNCTION NAME: initialize_rooms()						*
*  										 				*
* DESCRIPTION: Initializes room structs					*
*							 							*
* ARGUMENTS: 											*
*	(1) rooms: an array of pointers to a room structs	*
*							 							*
* RETURNS: Nothing										*
*							 							*
********************************************************/
void initialize_rooms(struct room* rooms[])
{
	int i, random;
	int option[] = { 0,1,2,3,4,5,6,7,8,9 };

	char* RoomNames[] = { "alpha", "beta", "gamma", "delta", 
		"epsilon", "zeta", "eta", "theta", "iota", "kappa" };

	srand((time(NULL)));

	for ( i = 0; i < 7; i++)
	{
		do
		{
			random = rand() % 10;
		} 
		while(option[random] == -1);

		option[random] = -1;	

		rooms[i]->id = i;
		rooms[i]->numberOfOutboundConnections = 0;
		rooms[i]->name = RoomNames[random];
	}
}

void give_rooms_types(struct room* rooms[])
{
	int i, random;
	int option[7] = { 0 };

	srand(getpid());

	random = (rand());

	printf("%d\n", random);

	random = (random % 7);
	printf("%d\n", random);

	rooms[random]->roomType = "START_ROOM";
	option[random] = -1;

	do
	{
		random = rand() % 7;
	} while(option[random] == -1);

	rooms[random]->roomType = "END_ROOM";
	option[random] = -1;


	for (i = 0; i < 7; i++)
	{
		if (option[i] == 0)
			rooms[i]->roomType = "MID_ROOM";
	}

	return;
}

void create_room_directory(struct room* rooms[])
{
	int Pid = getpid();
	
	char DirectoryName[20];
	memset(DirectoryName, '\0', 20);
	char* Prefix = "tibbsm.rooms.";
	sprintf(DirectoryName, "%s%d", Prefix, Pid);

	mkdir(DirectoryName, 0755);

	char filePath[100];

	int i, j, k;
	for ( i = 0; i < 7; i++)
	{
		memset(filePath, '\0', 20);
		sprintf(filePath, "./%s/%s_room", DirectoryName, rooms[i]->name);		
		// printf("%s\n", filePath);

		FILE* fp = fopen(filePath, "w");
		if (fp == NULL)
		{
			fprintf(stderr, "Could not open %s\n", filePath);
			perror("Error in create_room_directory()");
			exit(1);
		}

		j = rooms[i]->numberOfOutboundConnections;
		fprintf(fp, "ROOM NAME: %s\n", rooms[i]->name);
		for ( k = 0; k < j; k++)
			fprintf(fp, "CONNECTION %d: %s\n", k + 1, rooms[i]->outboundConnections[k]->name);
		fprintf(fp, "ROOM TYPE: %s\n", rooms[i]->roomType);

		fclose(fp);
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

