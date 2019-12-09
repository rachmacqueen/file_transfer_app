// defining struct to hold a socket's IP address information
#ifndef NETWORKING_H
#define NETWORKING_H
struct sockaddr_in;
#endif

// function for initiating client connection to server
int serverConnection(char *serverIP, int serverPort, int socket);

// function for receving the Message of the Day from the server
void getMOTD(int socket);

// function to send data across the network
int send_data(int socket, char buffer[1024], int len, int flag);

// function to receive data being sent across the network
int recv_data(int socket, char buffer[1024], int len, int flag);

// function for uploading/downloading file from server
void send_file(int socket, char filename[256], char directory[256]);
