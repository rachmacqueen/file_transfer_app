// function for simple shift cipher encryption
void encrypt(char pword[], int key);

// function for simple shift cipher decryption
void decrypt(char pword[], int key);

// function for receiving client password input and checking credentials
int check_authentication(int socket);

// function for prompting client password input and sending credentials to server
void client_authentication(int socket, char *username);


