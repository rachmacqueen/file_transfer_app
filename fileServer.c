#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "logger.h"
#include "IO.h"
#include "config.h"
#include "networking.h"
#include "authentication.h"
#include "signals.h"


int main()
{
	check_server_config();		// check if config file exists/create file and default variables for server.conf --> found in config.c file
	struct server_config sc;
	sc = load_server_config();		// load config variables in from server.conf file --> found in config.c file
			
	struct sockaddr_in serverAddress, clientAddress;
	
	// signal handling --> found in signals.c file
	signal(SIGINT, sig_handler);
        signal(SIGTERM, sig_handler);
        signal(SIGQUIT, sig_handler);
        signal(SIGHUP, sig_hup);  
	
	int listenSocket, clientSocket, r, m, a;
	unsigned int len;
	int val=1;
	char buffer[1024];
	char request [256];
	pid_t childpid;
	
	#define serverIP sc.ip
	#define serverPort sc.port
			
	printf("\nServer running on IP: %s and Port: %d\n\n", serverIP, serverPort);
	
	// socket creation and verification
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket < 0)
	{
		perror("Error during socket creation. The server will now exit.");
		exit(1);
	}
	else
        {
		printf("Socket successfully created.\n"); 
	}
	
	if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("Error whilst setting socket options. The server will now exit.");
		exit(1);
	}
	bzero(&serverAddress, sizeof(serverAddress));
	
	// IP, PORT assigning
	serverAddress.sin_family = AF_INET;
	inet_pton(AF_INET, serverIP , &serverAddress.sin_addr);
    	serverAddress.sin_port = htons(serverPort);
	
	// binding new socket to IP and verification
	if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0)
    	{
        	perror("Error during socket binding. The server will now exit.");
		exit(1);
    	}
	else
        {
		printf("Socket successfully binded.\n"); 
	}
	
	// server listening
	if ((listen(listenSocket, 12)) != 0)
	{
		printf("Listening socket failed. The server will now exit.");
		exit(1);
	}
	else
	{
		printf("\nWaiting for client connection.\n");
	}
				
	len = sizeof(clientAddress);

	// client_id variable used to distinguish between different client's entries in logfile.txt
	int client_id = 0;

	// enter accept loop for each new connection
	while(1) 	
	{
		client_id++;
		
		// accepting new client
		clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddress, &len);

		if(clientSocket < 0)
		{
    			perror("Error whilst accepting client connection.\n");
			exit(1);
		}
		else
		{
			printf("Server accepted the client.\n");
		}

		// fork process for each new client
		if ((childpid = fork()) == 0)
		{
			close(listenSocket);

			// function for password authentication --> found in authentication.c
			// DEFAULT PASSWORD IS SET TO 'password'
			if ((a = check_authentication(clientSocket)) == 0)
			{
				printf("Client authentication failed.\n");
				close(clientSocket);
				printf("\nWaiting for client connection.\n");
				continue;
			}


			else
			{
				client_connected_log(client_id);		// function for new client connected logfile entry  --> found in logger.c
				// if client is authenticated, send Message of the Day
				r = write(clientSocket, sc.motd, 256);
				if (r < 0)
				{
					perror("Error whilst writing to socket");
				}		
			}       
						
			while(1)
			{
				printf("\nWaiting for client input\n");
				
				if((m = recv_data(clientSocket, request, 256, 0)) == 0)
				{
					printf("Connection to client CLOSED\n");
					// go wait for new client
					break;
				}
				
				trim_end(request);
				buffer[0]='\0';		// reset variable
				if (strncmp("l", request, 1) == 0) 
				{
					printf("Sending directory list to client\n");
					// function for listing files in default directory ./fileshare ---> found in IO.c file
					list_server_dir(clientSocket);	
					fflush(stdout);			// make sure everything makes it to the output
					list_log(client_id, dir_path()); 		// function for directory listing logfile entry  --> found in logger.c
					request[0] = '\0';		// reset variable
					continue;
					
				}
				else if (strncmp("d", request, 1) == 0) 
				{
					
					char filename[256];
					printf("Waiting to receive filename request from client\n");
					r = recv_data(clientSocket, filename, 256, 0);
					// function for sending requested download file to client --> found in networking.c file
					send_file(clientSocket, filename, dir_path());
					download_log(client_id, filename); 		// function for client downloading file logfile entry  --> found in logger.c
					fflush(stdout);			// make sure everything makes it to the output
					filename[0] = '\0';		// reset variables
					request[0] = '\0';
					continue;
					
				}
				else if (strncmp("u", request, 1) == 0) 
				{
					char filename[256];
					char directory[256];
					strncpy(directory, dir_path(), 256);
					buffer[0] = '\0';		// double check variable is empty
					printf("Reading filename from client\n");
					r = recv_data(clientSocket, filename, 256, 0);
					printf("Waiting to recieve client file upload..\n");
					while ((m = recv_data(clientSocket, buffer, 1024, 0)) > 0)
					{
						if(memcmp(buffer, "The file does not exist/could not be found", 43) == 0 ) 
						{
							printf("%s\n", buffer);
							buffer[0] = '\0';		// reset variable
							r = write(clientSocket, "\nFile upload was unsuccessful\n", 1024);
							break;
						}
						else
						{
							printf("File received from client.\n");
							// function for saving received upload file from client --> found in IO.c file
							save_download(buffer, filename, directory);
							fflush(stdout);			// make sure everything makes it to the output
							buffer[0] = '\0';		// reset variable
							r = write(clientSocket, "\nFile upload successful\n", 1024);
							printf("File successfully uploaded.\n");			
						}
						break;
					}
					upload_log(client_id, filename); 	// function for client uploading file logfile entry  --> found in logger.c
					filename[0] = '\0';		// reset variables
					request[0] = '\0';
					continue;
				}
				else if (strncmp("q", request, 1) == 0) 
				{
					client_disconnected_log(client_id); 	// function for client disconnecting logfile entry  --> found in logger.c
					close(clientSocket);
					request[0] = '\0';		// reset variable
					break;
				}
				else 
				{
					request[0] = '\0';		// reset variable
					continue;
				}
				
			}// end of inner while loop
		}	
		close(clientSocket);
	}// end of outer while loop
	end_log();		// function for directory error logfile entry  --> found in logger.c
	close(listenSocket);
	return (0);
}







