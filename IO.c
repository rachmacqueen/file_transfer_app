#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "logger.h"
#include "config.h"
#include "networking.h"


// function for retrieving default fileshare name from server.config file
char* dir_path(void) 
{
	struct server_config sc;		// config struct defined in config.h file
	sc = load_server_config();
	static char dir_name[256];
	strncpy(dir_name, sc.directory, sizeof sc.directory);
	return dir_name;
}

// function for retrieving downloads directory name from settings.config file
char* download_dir(void) 
{
	struct client_config cc;		// config struct defined in config.h file
	cc = load_client_config();
	static char download_dir[256];
	strncpy(download_dir, cc.downloads, sizeof cc.downloads);
	return download_dir;
}

// function for creating filepath from filename + directory name
char* get_filepath(char filename[256], char directory[256])
{
	static char file_path[256];
	int length = strlen(filename);
	file_path[0] = '\0';   		// double check variable is empty
    	strcat(file_path, directory);
	strcat(file_path, "/");
    	strcat(file_path, filename);
	if (strncmp(filename + length - 4, ".txt", 4) == 0)
	{
	}
	else
	{
		strcat(file_path, ".txt");
	}
	return file_path;
}

// function for opening a file
void open_file(char *filePath, char filename[256], char buffer[1024])
{
	FILE *contents;
	int errno;
	contents = fopen(filePath, "r");
	if(errno == 2) 	// error number 2 of perror is "the file does not exist/could not be found"
	{	
		perror("File error");
		int errno;
		
		if((contents = fopen(filePath, "w")) != NULL)
		{
			fputs(buffer, contents); 
			printf("Saved downloaded file: %s\n", filename);
			fflush(stdout);		// make sure everything makes it to the output
			errno = 0; 	// reset errno variable to 0 for next loop
		}		
		else
		{
			printf("An error has ocurred. File could not be uploaded.\n");
			return;
		}
		fclose(contents);
	}
	else	// if no error occurs, print the file contents
	{
		printf("\nFile already exists. It will be overwritten with the new file.\n");
		if((contents = fopen(filePath, "w")) != NULL)
		{
			fputs(buffer, contents); 
			printf("Saved downloaded file: %s\n", filename);
			fflush(stdout);			// make sure everything makes it to the output
		}
		else
		{
			printf("An error has ocurred. File could not be uploaded.\n");
			return;
		}
		fclose(contents);						
	}	
}

// function for saving file from being sent from the server
void save_download(char buffer[1024], char filename[256], char directory[256]) 
{
	while(1) 
	{
		
		struct stat status;
		stat(directory, &status);
		char file_path[256];
		if (S_ISDIR(status.st_mode)) 	// if default directory exists
		{
			strncpy(file_path, get_filepath(filename, directory), 256);	// function for creating filepath from filename + directory name --> found above (in IO.c)
			printf("Download directory path: %s\n", file_path);
			open_file(file_path, filename, buffer);
			return;	
		}	
		else
		{ 
			printf("Unable to open the downloads directory %s.\n", directory);			
			perror("");
			printf("\nCreating directory now...\n");
			mkdir(directory, 0700);
			DIR *dir_pointer = opendir(directory);
			if (dir_pointer == NULL) 
			{
				perror("An error has occurred. The program will now exit.");
				closedir(dir_pointer);
				exit(1);
			}
			closedir(dir_pointer);		
			
			strncpy(file_path, get_filepath(filename, directory), 256);	// function for creating filepath from filename + directory name --> found above (in IO.c)
			open_file(file_path, filename, buffer);
			return;
		}
	}
}


// function for listing files in default directory
void list_server_dir(int socket) 
{
	while(1) 
	{
		struct stat status;
		stat(dir_path(), &status);
		if (S_ISDIR(status.st_mode)) 	// if default directory exists, open and list files
		{    	
			struct dirent *directory;
			DIR *dir_pointer;
			int errno;
			char dir_list [1024];
			dir_pointer = opendir(dir_path());
			dir_list[0] = '\0';   		// double check variable is empty
			strcat(dir_list, "\nDirectory found! Listing files... \n");		
			
			while ((directory = readdir(dir_pointer)) != NULL) 
			{
				strcat(dir_list, directory->d_name);
				strcat(dir_list, "\n");
			}
			strcat(dir_list, "*End of directory*\n");
			
			send_data(socket, dir_list, strlen(dir_list), 0);
			dir_list[0] = '\0';		// reset variable
			closedir(dir_pointer);			
		}	
	
		else 	//else, make the default directory - if that fails, exit the program
		{
			printf("Unable to open the directory %s.\n", dir_path());			
			perror("");
			send_data(socket, "No such file or directory", 26, 0);
			error_log(dir_path());		// function for directory error logfile entry  --> found in logger.c
			printf("\nCreating directory now...\n");
			mkdir(dir_path(), 0700);
			DIR *dir_pointer = opendir(dir_path());
			if (dir_pointer == NULL) 
			{
				perror("An error has occurred. The program will now exit.");
				error_log(dir_path());		// function for directory error logfile entry  --> found in logger.c
				closedir(dir_pointer);
				exit(1);
			}	
			closedir(dir_pointer);
			return;
		}	
		break;
	}
	return;	
}



// list files in the client's current working directory function
void list_client_dir(void)
{
	char cwd[PATH_MAX];
	struct stat status;
	stat(getcwd(cwd, sizeof(cwd)), &status);		// getcwd refers to the current working directory
	if (S_ISDIR(status.st_mode))
    	{    	
		struct dirent *directory;
		DIR *dir_pointer;
		dir_pointer = opendir(getcwd(cwd, sizeof(cwd)));
		printf("\nThe current directory is: %s \n\nListing files... \n", getcwd(cwd, sizeof(cwd)));
			
		while ((directory = readdir(dir_pointer)) != NULL) 
		{
			int length = strlen(directory->d_name);
			if (strcmp(directory->d_name, ".") != 0 && strcmp(directory->d_name, "..") != 0 && 		// list all txt files except logfile.txt
				strncmp(directory->d_name + length - 4, ".txt", 4) == 0 && strcmp(directory->d_name, "logfile.txt") != 0)	
        		{
	        		printf("[%s]\n", directory->d_name);
        		} 			
		}
		printf("*End of directory*\n");
		fflush(stdout);			// make sure everything makes it to the output
		closedir(dir_pointer);
			
	}	
	
	else 	
	{
		printf("Unable to read the current directory %s", dir_path());			
		perror("");
	}
	return;		
}












