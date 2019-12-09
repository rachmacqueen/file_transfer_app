//defining struct for config variables to be placed into
#ifndef CONFIG_H
#define CONFIG_H
struct client_config {
	char username[256];
	int serverPort;
	char serverIP[256];
	char downloads[256];	
};

struct server_config {
	int port;
	char ip[256];
	char directory[256];
	char logfile[256];
	char motd[1024];
};
#endif 

//trim function
char* trim_end(char* s);

//function for checking if client config file exists/creating file and default variables for setting.conf
void check_client_config(void);

//function for checking if server config file exists/creating file and default variables for server.conf
void check_server_config(void);

//function to load config variables from settings.conf into struct
struct client_config load_client_config(void);

//function to load config variables from server.conf into struct
struct server_config load_server_config(void);
