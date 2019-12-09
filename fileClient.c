#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "IO.h"
#include "config.h"
#include "networking.h"
#include "authentication.h"

int main()
{
	check_client_config();		// check if config file exists/create file and default variables for setting.conf --> found in config.c file
	struct client_config cc;
	cc = load_client_config();		// load config variables in from settings.conf file --> found in config.c file
	#define serverIP cc.serverIP
	#define serverPort cc.serverPort
	#define username cc.username
	
	// socket creation and verification
	int connectionSocket = socket(AF_INET, SOCK_STREAM, 0);		// initiate client connection to server function --> found in networking.c
	serverConnection(serverIP, serverPort, connectionSocket);
	
	client_authentication(connectionSocket, username);		// DEFAULT PASSWORD IS SET TO 'password'
	
	// display MOTD
	getMOTD(connectionSocket);		// receive the Message of the Day from the server function --> found in networking.c
	
	while(1)
	{
		int m;
		char buffer[1024];
		char response[256];		
		printf("\nPlease press \n'l' to request a list of shared files from the server \n'd' to download a particular file \n'u' to upload a file to the server \nor 'q' to quit the program. \n");
		fgets(response, 255, stdin);		
		trim_end(response);		
		if (strncmp("q", response, 1) == 0) 
		{
			send_data(connectionSocket, response, sizeof(response), 0);		
			printf("\nThe program will now exit..\n");
			close(connectionSocket);
			exit(1);
		}
		else if (strncmp("l", response, 1) == 0) 
		{
			send_data(connectionSocket, response, sizeof(response), 0);
			printf("Waiting to receive list from server\n");
			
			while ((m = recv_data(connectionSocket, buffer, 1024, 0)) > 0) 
			{
				if(memcmp(buffer, "No such file or directory", 26) == 0 ) 
					{
						printf("\n%s\n", buffer);
						buffer[0] = '\0';		// reset variable
						break;
					}
				else
				{	
					buffer[m] = '\0';		// explicit null termination
					printf("\n%s\n", buffer);
					fflush(stdout);			// make sure everything makes it to the output
				}
				break;
			}
			buffer[0] = '\0';		// reset variables
			response[0]='\0';
			continue;			
		}
		else if (strncmp("d", response, 1) == 0) 
		{
			send_data(connectionSocket, response, sizeof(response), 0);
			char filename[256];
			char directory[256];
			strncpy(directory, download_dir(), 256);	
			printf("\nPlease enter the name of the file you wish to download: ");   
			fgets(filename, 1024, stdin);
			trim_end(filename);
			send_data(connectionSocket, filename, sizeof(filename), 0);	
			fflush(stdout);			// make sure everything makes it to the output
			buffer[0] = '\0';		// reset variable
			while ((m = recv_data(connectionSocket, buffer, 1024, 0)) > 0)
			{
				if(memcmp(buffer, "The file does not exist/could not be found", 43) == 0 ) 
				{
					printf("\n%s\n", buffer);
					buffer[0] = '\0';		// reset variable
					break;
				}
				else
				{
					printf("\nDownloading file %s from server\n", filename);
					printf("\nFile contents: \n%s\n", buffer);
					save_download(buffer, filename, directory);		// function for saving file from server --> found in IO.c
					fflush(stdout);			// make sure everything makes it to the output
								
				}
				break;
			}
			buffer[0] = '\0';		// reset variables
			filename[0] = '\0';	
			response[0]='\0';
			continue;
		}
		else if (strncmp("u", response, 1) == 0) 
		{
			send_data(connectionSocket, response, sizeof(response), 0);
			while(1)
			{
				char input[256];
				printf("\nYou can upload a .txt file from your current directory. Please press 'l' if you wish to see a list of available files, or any key to continue.\n");
				fgets(input, 255, stdin);
				trim_end(input);
				if (strncmp("l", input, 1) == 0) 
				{
					list_client_dir();		// lists files in the client's current working directory function --> found in IO.c
					continue;
				}		
				else
				{
					break;
				}
			}
			char filename[256];
			printf("\nPlease enter the name of the file you wish to upload: ");   
			fgets(filename, 1024, stdin);
			trim_end(filename);
			send_data(connectionSocket, filename, sizeof(filename), 0);	
			buffer[0] = '\0';			// reset variable
			char cwd[PATH_MAX];
			send_file(connectionSocket, filename, getcwd(cwd, sizeof(cwd)));	// function for uploading/downloading file from server --> found in networking.c

			m = recv_data(connectionSocket, buffer, 1024, 0);
			printf("%s\n", buffer);
			buffer[0] = '\0';		// reset variables
			filename[0] = '\0';		
			response[0]='\0';
			continue;			
		}
		
		else 
		{
			printf("Please enter one of the given options.\n");
		}
	close(connectionSocket);
	}
	return 0;
}















