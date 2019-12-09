#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "config.h"

// function for loading logfile path from server.config file
char* load_logpath(void) 
{
	struct server_config sc;		// config struct defined in config.h file
	sc = load_server_config();
	static char log_path[256];
	strncpy(log_path, sc.logfile, sizeof sc.logfile);
	return log_path;
}
 
// function for program startup logfile entry in new or existing logfile
void start_log(void) 
{
	struct server_config sc;		// config struct defined in config.h file
	sc = load_server_config();
	static char serverIP[256];
	int serverPort = sc.port;
	strncpy(serverIP, sc.ip, sizeof sc.ip);
	FILE *logfile = fopen("logfile.txt", "w"); 	// opening in write mode automatically creates a new logfile for each instance of the program
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Started server on %s:%d \n", ctime(&now), serverIP, serverPort); 
	fclose(logfile);
	printf("*Logfile created.*\n");
}

// function for new client connected logfile entry in new or existing logfile
void client_connected_log(int client_id) 
{
	FILE *logfile = fopen(load_logpath(), "a"); 	
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Client (ID: %d) connected. \n", ctime(&now), client_id); 
	fclose(logfile);
	printf("*Logfile updated.*\n");
}

// function for client disconnecting logfile entry in new or existing logfile
void client_disconnected_log(int client_id) 
{
	FILE *logfile = fopen(load_logpath(), "a"); 	
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Client (ID: %d) disconnected. \n", ctime(&now), client_id); 
	fclose(logfile);
	printf("*Logfile updated.*\n");
}

// function for server config file creation logfile entry in new or existing logfile	
void config_log(void)
{
	FILE *logfile = fopen(load_logpath(), "a"); 
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Created new server.conf file as config file not found. \n", ctime(&now)); 
	fclose(logfile);
	printf("*Logfile updated.*\n");
}

	
// function for directory listing logfile entry in new or existing logfile
void list_log(int client_id, char directory[])
{
	FILE *logfile = fopen(load_logpath(), "a");
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Client (ID: %d) viewed list of files in the directory %s\n", ctime(&now), client_id, directory); 
	fclose(logfile);
	printf("*Logfile updated.*\n");
}	

// function for client downloading file logfile entry in new or existing logfile
void download_log(int client_id, char filename[256]) 
{
	FILE *logfile = fopen(load_logpath(), "a");
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Client (ID: %d) downloaded file %s from the server\n", ctime(&now), client_id, filename); 
	fclose(logfile);
	printf("*Logfile updated.*\n");
}

// function for client uploading file logfile entry in new or existing logfile
void upload_log(int client_id, char filename[256]) 
{
	FILE *logfile = fopen(load_logpath(), "a");
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Client (ID: %d) uploaded file %s to the server\n", ctime(&now), client_id, filename); 
	fclose(logfile);
	printf("*Logfile updated.*\n");
}	

// function for directory error logfile entry in new or existing logfile
void error_log(char directory[])
{
	FILE *logfile = fopen("logfile.txt", "a");
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Error encountered whilst accessing file/directory %s\n", ctime(&now), directory); 
	fclose(logfile);
}

// function for program termination logfile entry in new or existing logfile
void end_log(void)
{
	FILE *logfile = fopen(load_logpath(), "a"); 
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: Server terminated.\n", ctime(&now)); 
	fclose(logfile);
	printf("*Logfile updated.*\n");
}

// function for signal handling logfile entry in new or existing logfile
void signal_log(void)
{
	FILE *logfile = fopen(load_logpath(), "a");
	time_t now;
	time(&now);
	fprintf(logfile, "\nTimestamp: %sDescription: SIGHUP signal received. Server config settings reloaded.\n", ctime(&now)); 
	fclose(logfile);
	printf("*Logfile updated.*\n");
}
