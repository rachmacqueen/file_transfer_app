#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include "logger.h"
#include "config.h"


// trim function
char* trim_end(char* str) 		// trims whitespace and newline character from a line
{
	int len = strlen(str);
	if(len == 0) 
	{
        	return str;
    	}
	char* position = str + len - 1;
    	while(position >= str && isspace(*position)) 
	{
        	*position = '\0';
        	position--;
    	}
    return str;
}

// function for checking if client config file exists/creating file and default variables for settings.conf
void check_client_config(void) 
{
	int errno;
	FILE *cf_in = fopen("settings.conf", "r");
		if(errno == 2) 	// if the file doesn't exist, create it and populate with default variables
		{	// error number 2 of perror is "the file does not exist/could not be found"
			perror("An error has occurred whilst opening the config file");
			FILE *cf_out = fopen("settings.conf", "w");
			fprintf(cf_out, "Username: guest \nServer Port: 1337 \nServer IP Address: 127.0.0.1 \nDownloads directory: ./downloads\n"); 
			printf("Created settings.conf file..\n");
			fclose(cf_out);		
		}
		else if(cf_in == NULL) 
		{
			perror("ERROR! The file could not be read. The program will now exit..\n");
			fclose(cf_in);
			exit(1);
		}
		else
		{
			fclose(cf_in);
		}
}

// function for checking if server config file exists/creating file and default variables for server.conf
void check_server_config(void) 
{
	int errno;
	FILE *cf_in = fopen("server.conf", "r");
		if(errno == 2) 	// if the file doesn't exist, create it and populate with default variables
		{	// error number 2 of perror is "the file does not exist/could not be found"
			perror("An error has occurred whilst opening the config file");
			FILE *cf_out = fopen("server.conf", "w");
			fprintf(cf_out, "Server Port: 1337 \nServer IP Address: 127.0.0.1 \nDefault directory: ./fileshare \nLogfile path: ./logfile.txt  \nMOTD: Welcome to the FileShare Program!\n"); 
			start_log();		// function for program startup logfile entry  --> found in logger.c
			printf("Created server.conf file..\n");
			fclose(cf_out);		
			config_log();		// function for server config file creation logfile entry  --> found in logger.c
		}
		else if(cf_in == NULL) 
		{
			perror("ERROR! The file could not be read. The program will now exit..\n");
			fclose(cf_in);
			struct server_config sc;
			sc = load_server_config();
			error_log(sc.directory);		// function for directory listing logfile entry  --> found in logger.c
			exit(1);
		}
		else
		{
			start_log();		// function for program startup logfile entry  --> found in logger.c
			fclose(cf_in);
		}
}


// function to load client config variables from settings.conf into struct
struct client_config load_client_config(void) 
{	
	struct client_config cc;
	FILE *config_file;
	config_file = fopen("settings.conf", "r");
	if (config_file == NULL) 
	{
		perror("Error! The file could not be read. The program will now exit...\n");
		fclose(config_file);	
		exit(1);
	}
	char line[1024] = {'\0'};		// double check variable is empty
	char port[256];
				
	// read + assign 1st line = username variable
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(cc.username, line+10, sizeof line);
	
	// read + assign 2nd line = server port variable			
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(port, line+13, sizeof line);
	cc.serverPort = atoi(port);

	// read + assign 3rd line = server ip variable			
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(cc.serverIP, line+19, sizeof line);
	
	// read + assign 4th line = download path variable			
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(cc.downloads, line+21, sizeof line);
				
	fclose(config_file);
	return cc;	
}

// function to load server config variables from server.conf into struct
struct server_config load_server_config(void) 
{	
	struct server_config sc;
	FILE *config_file;
	config_file = fopen("server.conf", "r");
	if (config_file == NULL) 
	{
		perror("Error! The file could not be read. The program will now exit...\n");
		fclose(config_file);	
		exit(1);
	}
	char line[1024] = {'\0'};		// double check variable is empty
	char port[256];
				
	// read + assign 1st line = server port variable			
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(port, line+13, sizeof line);
	sc.port = atoi(port);

	// read + assign 2nd line = server ip variable			
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(sc.ip, line+19, sizeof line);
	
	// read + assign 3rd line = directory path variable
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(sc.directory, line+19, sizeof line);
	
	// read + assign 4th line = logfile path variable		
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(sc.logfile, line+14, sizeof line);
	
	// read + assign 5th line = motd variable			
	fgets(line, sizeof line, config_file);
	trim_end(line);
	strncpy(sc.motd, line+6, sizeof line);
				
	fclose(config_file);
	return sc;	
}



