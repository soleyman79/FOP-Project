#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAX 80 
#define PORT 12345
#define SA struct sockaddr 

// Function designed for chat between client and server. 
void chat(int server_socket) 
{ 
	char buffer[MAX]; 
	int n; 
	while (true) { 
		bzero(buffer, sizeof(buffer)); 
		printf("Enter your message: "); 
		n = 0;

		// Copy client message to the buffer
		while ((buffer[n++] = getchar()) != '\n') 
			;

		// Send the buffer to server
		send(server_socket, buffer, sizeof(buffer), 0);
		bzero(buffer, sizeof(buffer));

		// Read the message from server and copy it to buffer
		recv(server_socket, buffer, sizeof(buffer), 0);

		// Print buffer which contains the server message
		printf("From server: %s", buffer);

		// If the message starts with "exit" then client exits and chat ends
		if ((strncmp(buffer, "exit", 4)) == 0) { 
			printf("Client stopping...\n"); 
			return; 
		} 
	} 
} 

int main() 
{ 
	int client_socket, server_socket; 
	struct sockaddr_in servaddr, cli; 

	// Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (client_socket == -1) { 
		printf("Socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

	// Assign IP and port
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 

	// Connect the client socket to server socket 
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("Connection to the server failed...\n"); 
		exit(0); 
	} 
	else
		printf("Successfully connected to the server..\n"); 

	// Function for chat 
	chat(client_socket); 

	// Close the socket 
	shutdown(client_socket, SHUT_RDWR); 
} 