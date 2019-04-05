/********************************************************
*                                                       *
* FILENAME:     otp_enc.c                               *
* AUTHOR:       Marc Tibbs                              *
* EMAIL:        tibbsm@oregonstate.edu                  *
* CLASS:        CS344 (Winter 2019)                     *
* ASSIGNMENT:   Program 4 - OTP                         *
*                                                       *
********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

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
* FUNCTION NAME: sendFile()                             *
*                                                       *
* DESCRIPTION: Sends file to the server                 *
*                                                       *
* ARGUMENTS:                                            *
*   (1) filename: The file to send to the server        *
*   (2) socketFD: The socket to use to send the file    *
*   (3) fileLength: The size of the file to send        *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
********************************************************/
void sendFile(char* name, int socketFD, int size)
{ 
    int charsWritten, length, total; 
    char buffer[BUFFER_SIZE]; 
    // Open the file 
    FILE* file = fopen(name, "r");         

    // Initialize buffer variable
    memset(buffer, '\0', sizeof(buffer)); 

    // Read data into the buffer and set length to the amount read
    length = fread(buffer, sizeof(char), size, file);      

    charsWritten = send(socketFD, buffer, length, 0);       
    // Set total to length read
    total = length;

    // Keep on sending until all data has been read
    while(length > 0 && total <= size)    
    {   

        fprintf(stderr, "%s - %d\n", name, length); 

        // Send message to server 
        charsWritten = send(socketFD, buffer, length, 0);       

        // Break if nothing to send
        if(charsWritten < 0) 
        {    
                break; 
        } 

        // Clear buffer
        memset(buffer, '\0', sizeof(buffer)); 

        // Read more data into the buffer and set length to the amount read
        length = fread(buffer, sizeof(char), size, file);

        // Recalculate total 
        total += length;             
    } 
    
    // Close the file and return   
    fclose(file);              
    return; 
} 

int main(int argc, char *argv[])
{
    int socketFD, portNumber, key, keySize, message, messageSize, fileCheck;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;

    char buffer[BUFFER_SIZE];
    char secret[] = "otp_enc";  

    if (argc != 4) 
    { 
        error("Error: wrong number of arguments.\n");  
    } 
    
    // Clear buffer
    memset(buffer, '\0', sizeof(buffer));

    // Open the file passed in 
    fileCheck = open(argv[1], O_RDONLY);

    // Check that the message is only alphabet or space chars
    while ( read(fileCheck , &buffer, sizeof(char)) > 0 )
    {
        if(!isspace(buffer[0]) && !isalpha(buffer[0]))
        {
            error("Error: input contains bad characters\n");
        }
    }

    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); 
    portNumber = atoi(argv[3]); 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(portNumber); 
    serverHostInfo = gethostbyname("localhost"); 

    // Exit on failure to connect with host
    if (serverHostInfo == NULL) 
    {
        error("Error: no such host, in client");
    }

    // Copy in the address
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); 
    
    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 

    // Exit if there was an error with the socket
    if (socketFD < 0) 
    {
        error("Error: error while opening socket, in client");
    }
    
    // Connect to server and exit on any error
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
        error("Error: error while connecting to socket, in client");
    }   
    
    // Send secret to server
    write(socketFD, secret, sizeof(secret));

    // Read response from server
    memset(buffer, '\0', sizeof(buffer));
    read(socketFD, buffer, sizeof(buffer));

    // Verify server sent correct reponse
    if (strcmp(buffer, "otp_enc_d") !=0)
    {
        error("Error: authentication failed\n");
    }

    // Read key and message files and get their sizes
    key = open(argv[2], O_RDONLY);
    message = open(argv[1], O_RDONLY);
    keySize = lseek(key, 0, SEEK_END);
    messageSize = lseek(message, 0, SEEK_END); 

    // Verify that key is larger that message
    if(keySize < messageSize)
    {   
        fprintf(stderr, "Error: key '%s' is too short\n", argv[2]);
        exit(1);
    }

    // Clear the buffer
    memset(buffer, '\0', sizeof(buffer));
    
    //Send message and key to server
    sendFile(argv[1], socketFD, messageSize);       
    sendFile(argv[2], socketFD, keySize);       

    char readBuffer[256];
    size_t bufferSize = 0;

    // Read message
    do 
    {
        memset(readBuffer, '\0', sizeof(readBuffer));

        if(bufferSize == sizeof(buffer)) 
        { 
            exit(1); 
        }

        // Get the next chunk of the message
        int r = recv(socketFD, readBuffer, sizeof(readBuffer), MSG_WAITALL);

        if(r < 0) 
        { 
            // error("Error while reading the message.\n"); 
            // fprintf(stderr, "recv: %s (%d)\n", strerror(errno), errno);
            break;
        }

        strcat(buffer, readBuffer);

        bufferSize += r;

    } while(bufferSize < sizeof(buffer));

    // Send received message to stdout
    fprintf(stdout, "%s\n", buffer);

    // Close the socket
    close(socketFD); 

    return 0;
}