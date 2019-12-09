// function for retrieving default fileshare name from server.config file
char* dir_path(void);

// function for retrieving downloads directory name from settings.config file
char* download_dir(void);

// function for creating filepath from filename + directory name
char* get_filepath(char filename[256], char directory[256]);

// function for opening a file
void open_file(char *filePath, char filename[256], char buffer[1024]);

// function for saving file from being sent from the server
void save_download(char buffer[1024], char filename[256], char directory[256]);

// function for listing files in default directory
int list_server_dir(int socket);

// list files in the client's current working directory function
void list_client_dir(void);












