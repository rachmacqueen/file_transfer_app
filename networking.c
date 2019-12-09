#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "networking.h"
#include "config.h"
#include "IO.h"
#include "logger.h"

// function for initiating client connection to server
int serverConnection(char *serverIP, int serverPort, int socket)
{
	struct sockaddr_in serverAddress;
	
	// client socket creation
    	if (socket < 0)
    	{
    		perror("Error during socket creation. The program will now exit.");
		exit(1);
    	}
	else
        {
		printf("Socket successfully created.\n");  
	}
    	
	// IP, PORT assigning
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, serverIP, &serverAddress.sin_addr);
	serverAddress.sin_port = htons(serverPort);

	// connect client socket to server socket
	if (connect(socket, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) < 0)
	{
		perror("Server Connection Error. The program will now exit.");
		exit(1);		
	}
	else
	{
	        printf("\nConnected to the server running on IP: %s and Port: %d\n\n", serverIP, serverPort); 
	}  
	return socket;
	
}

// function for receving the Message of the Day from the server
void getMOTD(int socket)
{
	
	int m;
	char buffer [1024];
	
	m = read(socket, buffer, sizeof(buffer));
	if (m < 0)
	{
		perror("Error whilst reading from socket");
	}

	printf("\n%s\n", buffer);
	buffer[0] = '\0';		// reset variable
	fflush(stdout);			// make sure everything makes it to the output
}		

// function to send data across the network
int send_data(int socket, char buffer[1024], int len, int flag)
{
	int r;	
	r = send(socket, buffer, len, flag);
	if (r < 0)
	{
		perror("Error whilst writing to socket");
	}
	fflush(stdout);			// make sure everything makes it to the output
	return r;
}

// function to receive data being sent across the network
int recv_data(int socket, char buffer[1024], int len, int flag)
{
	int m;
	m = recv(socket, buffer, len, flag);
	if (m < 0)
	{
		perror("Error whilst reading from socket");
	}
	return m;
}

// function for uploading/downloading file from server
void send_file(int socket, char filename[256], char directory[256])
{
	char line[640], message[1024], file_path[256];	
	int errno;
	strncpy(file_path, get_filepath(filename, directory), 256);		// function for creating filepath from filename + directory name --> found in IO.c
	printf("Filepath: %s \n", file_path);
	
	FILE *contents;
	contents = fopen(file_path, "r");
		
	printf("\nDirectory: %s \n", directory);		
	printf("Filename: %s \n\n", filename);
	if(errno == 2)	// error number 2 of perror is "the file does not exist/could not be found"
	{	
		perror("An error has occurred ");
		send(socket, "The file does not exist/could not be found", 43, 0);
		error_log(filename);		// function for directory error logfile entry  --> found in logger.c
		errno = 0; 	//reset errno variable to 0 for next loop
	}
	else	// if no error occurs, print the file contents
	{
		message[0] = '\0';   // double check variable is empty
		while(fgets(line, sizeof(line), contents) != NULL) 
		{
			printf("%s", line);
    			strcat(message, line);
			strcat(message, "\0");
		}
		send_data(socket, message, sizeof(message), 0);
		line[0] = '\0';		// reset variables
		message[0] = '\0';		
		fclose(contents);						
	}
	file_path[0] = '\0';		// reset variable
	return;
}











