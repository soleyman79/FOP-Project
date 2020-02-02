#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/ip.h> 
#define MAX 80 
#define PORT 12345
#define SA struct sockaddr 

// Function designed for chat between client and server. 
void chat(int client_socket) 
{ 
	char buffer[MAX]; 
	int n; 
	while (true) { 
		bzero(buffer, sizeof(buffer)); 

		// Read the message from client and copy it to buffer 
		recv(client_socket, buffer, sizeof(buffer), 0); 

		// Print buffer which contains the client message
		printf("From client: %s\t To client : ", buffer); 
		bzero(buffer, sizeof(buffer)); 
		n = 0; 
		// Copy server message to the buffer 
		while ((buffer[n++] = getchar()) != '\n') 
			; 

		// Send the buffer to client 
		send(client_socket, buffer, sizeof(buffer), 0); 

		// If the message starts with "exit" then server exits and chat ends
		if (strncmp("exit", buffer, 4) == 0) { 
			printf("Server stopping...\n"); 
			return; 
		} 
	} 
} 

// Driver function 
int main() 
{ 
	int server_socket, client_socket; 
	struct sockaddr_in server, client; 

	// Create and verify socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (server_socket == -1) { 
		printf("Socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

	// Assign IP and port
	bzero(&server, sizeof(server)); 
	server.sin_family = AF_INET; 
	server.sin_addr.s_addr = htonl(INADDR_ANY); 
	server.sin_port = htons(PORT); 

	// Bind newly created socket to given IP and verify
	if ((bind(server_socket, (SA*)&server, sizeof(server))) != 0) { 
		printf("Socket binding failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully bound..\n"); 

	// Now server is ready to listen and verify 
	if ((listen(server_socket, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 

	// Accept the data packet from client and verify
	socklen_t len = sizeof(client);
	client_socket = accept(server_socket, (SA*)&client, &len); 
	if (client_socket < 0) { 
		printf("Server accceptance failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server acccepted the client..\n"); 

	// Function for chatting between client and server 
	chat(client_socket); 

	// Close the socket 
	shutdown(server_socket, SHUT_RDWR); 
} 