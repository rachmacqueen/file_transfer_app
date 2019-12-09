// function for loading logfile path from server.config file
char* load_logpath(void);

// function for program startup logfile entry in new or existing logfile
void start_log(void);

// function for new client connected logfile entry in new or existing logfile
void client_connected_log(int client_id); 

// function for client disconnecting logfile entry in new or existing logfile
void client_disconnected_log(int client_id); 

// function for server config file creation logfile entry in new or existing logfile	
void config_log(void);

// function for directory listing logfile entry in new or existing logfile
void list_log(int client_id, char directory[]);

// function for client downloading file logfile entry in new or existing logfile
void download_log(int client_id, char filename[256]);

// function for client uploading file logfile entry in new or existing logfile
void upload_log(int client_id, char filename[256]); 

// function for directory error logfile entry in new or existing logfile
void error_log(char directory[]);

// function for program termination logfile entry in new or existing logfile
void end_log();

// function for signal handling logfile entry in new or existing logfile
void signal_log(void);
