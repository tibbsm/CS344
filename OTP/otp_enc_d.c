/********************************************************
*                                                       *
* FILENAME:     otp_enc_d.c                             *
* AUTHOR:       Marc Tibbs                              *
* EMAIL:        tibbsm@oregonstate.edu                  *
* CLASS:        CS344 (Winter 2019)                     *
* ASSIGNMENT:   Program 4 - OTP                         *
*                                                       *
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 75000 

/********************************************************
*                                                       *
* FUNCTION NAME: error()                                *
*                                                       *
* DESCRIPTION: Prints error message and exit            *
*                                                       *
* ARGUMENTS:                                            *
*   (1) msg: The error message to print                 *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
********************************************************/
void error(const char *msg) 
{ 
    fprintf(stderr, "%s\n", msg); 
    exit(1); 
} 


/********************************************************
*                                                       *
* FUNCTION NAME: charToInt()                            *
*                                                       *
* DESCRIPTION: Converts a char to an int value          *
*                                                       *
* ARGUMENTS:                                            *
*   (1) c: The char to conver to a integer              *
*                                                       *
* RETURNS: A int value of the passed in char            *
*                                                       *
********************************************************/
int charToInt(char c)
{
    if (c == ' ')
    {
        return 26;
    }   
    else
    {
        return c - 'A';
    }
}

/********************************************************
*                                                       *
* FUNCTION NAME: intToChar()                            *
*                                                       *
* DESCRIPTION: Converts an int to a character           *
*                                                       *
* ARGUMENTS:                                            *
*   (1) i: The int to convert to a character            *
*                                                       *
* RETURNS: The char value of the passed in int          *
*                                                       *
********************************************************/
char intToChar(int i)
{
    if (i == 26)
    {
        return ' ';
    }
    else 
    {
        return i + 'A';
    }
}

int main(int argc, char *argv[])
{
    int listenSocketFD, childConnectionFD, portNumber, charsRead;
    socklen_t sizeOfClientInfo;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serverAddress, clientAddress;
    pid_t pid;  

    // Check number of arguments
    if (argc < 2) 
    { 
        error("Error: wrong number of arguments.\n");
    } 
    
    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); 
    portNumber = atoi(argv[1]); 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(portNumber); 
    serverAddress.sin_addr.s_addr = INADDR_ANY; 

    // Set up the socket
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); 

    // Exit if there was an error with the socket
    if (listenSocketFD < 0)
    {
        error("Error: there was an error opening the socket.\n");
    }

    // Enable the socket to begin listening. Exit on any error
    if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
    {
        error("Error: there was an error on binding the socket.\n");
    }

    // Turn the socket on 
    listen(listenSocketFD, 5); 
    
    while(1)
    {    
        // Get the size of the address for the client that will connect
        sizeOfClientInfo = sizeof(clientAddress); 

        // Accept a connection, blocking if one is not available until one connects
        childConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);


        if (childConnectionFD < 0) 
        {
            fprintf(stderr, "ERROR on accept");
        }

        // Fork 
        pid = fork();

        // Exit if there was an error while forking
        if(pid < 0)
        {
            fprintf(stderr, "Error forking\n");
            exit(1);
        }

        // Child process
        if(pid == 0) 
        {
            // Clear buffer
            memset(buffer, '\0', BUFFER_SIZE);

            // Pointer to buffer postion
            char* ptr = buffer; 

            // Pointer to keys postions
            char* keyStart;
            char* keyEnd;

            // Variable to track the new lines in the message
            int newLine = 0;

            // Variable to track the amount left to read
            int toRead = BUFFER_SIZE;

            // Track the amount of the message read
            int totalRead = 0;

            // Flag to keep reading message
            int reading = 1;

            int i;
            
            char message[BUFFER_SIZE];
            char key[BUFFER_SIZE];
            
            // Message length
            int length = 0;
            
            // Authenticate client 
            read(childConnectionFD, buffer, sizeof(buffer));

            // If client sent incorrect secret send back error response
            if( strcmp(buffer, "otp_enc") !=0)
            {
                char response[] = "Authentication failed\n";
                write(childConnectionFD, response, sizeof(response)); 
                exit(1);
            }
            // Send back authentication success response
            else
            {
                char response[] = "otp_enc_d";
                write(childConnectionFD, response, sizeof(response));
            }

            // Clear buffer
            memset(buffer, '\0', sizeof(buffer));
    
            // Loop over stream data to get key and message
            while(reading)
            {   
                // Read message 
                charsRead = recv(childConnectionFD, ptr, toRead, 0); 
                
                if (charsRead < 0)
                {
                    fprintf(stderr, "ERROR reading from socket"); 
                    exit(2);
                }
                
                if (charsRead == 0)
                {
                    break;
                }
            
                //Step message to find new line separating key and message
                for (i = totalRead; i < totalRead + charsRead; i++)
                {   
                    if (buffer[i] == '\n')
                    {
                        newLine++;

                        // Second newline; Start of key
                        if (newLine == 1)
                        {
                            // Set pointer to start of key
                            keyStart = buffer + i + 1;   
                            length = keyStart - buffer;
                        }

                        // Second newline; End of key
                        else if (newLine == 2)         
                        {
                            // Set pointer to end of key
                            keyEnd = buffer + i;   

                            // Set reading flag to false
                            reading = 0;
                            break;
                        }
                    }
                }

                totalRead += charsRead;
                ptr += charsRead;
                toRead -= charsRead;
            }

            //Copy data into char arrays
            memset(key, '\0', sizeof(key));
            strncpy(key, keyStart, (keyEnd - keyStart));  
            
            memset(message, '\0', sizeof(message));
            strncpy(message, buffer, (keyStart - buffer));
            
            // Encrypt the message
            for (i = 0; i < length - 1; i++)
            {
                message[i] = intToChar((charToInt(message[i]) + charToInt(key[i])) % 27);
            }   

            // Terminate the message with null
            message[i] = '\0';

            // Send encoded  message back to the client
            charsRead = send(childConnectionFD, message, sizeof(message), 0);

            if (charsRead < 0) 
            {
                error("Error: error while writing to socket\n"); 
            }

            close(childConnectionFD); 
            exit(0);
        }
        else
        {
            close(childConnectionFD);
            // Kill zombies
            wait(NULL);
        }        
    }

    // Close the listening socket
    close(listenSocketFD); 
    return 0; 
}