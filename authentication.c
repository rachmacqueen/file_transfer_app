#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "networking.h"

// function for simple shift cipher encryption
void encrypt(char pword[], int key)
{
	unsigned int i;
	for(i=0;i<strlen(pword);++i)
	{
        	pword[i] = pword[i] - key;
	}
}

// function for simple shift cipher decryption
void decrypt(char pword[], int key)
{
    	unsigned int i;
    	for(i=0;i<strlen(pword);++i)
    	{
          	pword[i] = pword[i] + key;
    	}
}

// function for receiving client password input and checking credentials
int check_authentication(int socket)
{
	char credentials[256];
	char *cred1, *cred2;
	char username[256], password[256];
	char response[256];
	int attempts=1, success=1, fail=0;
 	int maxTry = 3;
 	while(attempts <= maxTry)
	{
		recv_data(socket, credentials, sizeof(credentials), 0);
		decrypt(credentials, 28);		// function for decrypting credentials --> found above (in authentication.c)
		cred1 = strtok(credentials, ":");
		cred2 = strtok(NULL, ":");
		strncpy(username, cred1, 256);
		strncpy(password, cred2, 256);
		decrypt(password, 33);		// decrypt double encryption	
		credentials[0] = '\0';		// clear the credentials memory
		if(memcmp(username, "guest", 5) == 0)
		{
   			if(memcmp(password, "password", 8) == 0)
			{
    				// authentication is successful
    				strncpy(response, "correct", 8);
				send_data(socket, response, sizeof(response), 0);
				response[0] = '\0';		// clear the response memory
				return success;
   			}
			else
			{
				// authentication is unsuccessful - MAX 3 TRIES
				strncpy(response, "incorrect", 10);
				send_data(socket, response, sizeof(response), 0);
				response[0] = '\0';		// clear the response memory
				attempts++;
				continue;
			}
 		}
		else
		{
   			// usernames failed to match
			response[0] = '\0';		// double check response is empty
			strncpy(response, "namefail", 9);
			send_data(socket, response, sizeof(response), 0);
			response[0] = '\0';		// clear the response memory
			return fail;
  		}
	}
	// authentication failed
 	response[0] = '\0';		// double check response is empty
	strncpy(response, "max", 4);
	send_data(socket, response, sizeof(response), 0);
	return fail;
}


// function for prompting client password input and sending credentials to server
void client_authentication(int socket, char *username)
{
	char credentials[256], response[256], uname[256];
	char *password;
	trim_end(username);
	strncpy(uname, username, 256);
	while(1)
	{		
		password = getpass("Enter password: ");		// getpass used so user input is not displayed in the terminal for added privacy/security
		encrypt(password, 33);			// function for enrypting credentials --> found above (in authentication.c)
		strcat(credentials, username);       	
		strcat(credentials,":");
       		strcat(credentials, password);
		encrypt(credentials, 28);		// double encryption for added security
       		send_data(socket, credentials, 256, 0);
		recv_data(socket, response, sizeof(response), 0);

		// authentication is successful
		if(memcmp(response, "correct", 8) == 0) 
		{
			printf("Authentication successful.\n");
			response[0] = '\0';		// reset variable
			return;
		}
		// authentication is unsuccessful - MAX 3 TRIES
		else if(memcmp(response, "incorrect", 10) == 0) 
		{
			printf("Authentication unsuccessful. Please try again.\n");
			response[0] = '\0';		// reset variables
			credentials[0] = '\0';		
			password[0]= '\0';
			continue;
		}
		// usernames failed to match
		else if(memcmp(response, "namefail", 9) == 0) 
		{
			printf("Username does not match. Please check config file and then try again.\n");
			response[0] = '\0';		/// reset variables
			credentials[0] = '\0';		
			password[0]= '\0';
			continue;
		}
		// authentication failed
		else if(memcmp(response, "max", 4) == 0) 
		{
			printf("Exceeded maximum amount of password attempts. The program will now exit.\n");
			response[0] = '\0';		// reset variable
			exit(1);
		}
		break;
	}
	return;
}   










