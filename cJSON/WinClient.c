#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <string.h> 
#include <winsock2.h>
#define MAX 80 
#define PORT 12345
#define SA struct sockaddr 

// Function designed for chat between client and server. 
void chat(int server_socket) 
{ 
	char buffer[MAX]; 
	int n; 
	while (true) { 
		memset(buffer, 0, sizeof(buffer)); 
		printf("Enter your message: "); 
		n = 0;

		// Copy client message to the buffer
		while ((buffer[n++] = getchar()) != '\n') 
			;

		// Send the buffer to server
		send(server_socket, buffer, sizeof(buffer), 0);
		memset(buffer, 0, sizeof(buffer));

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
	
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

	// Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (client_socket == -1) { 
		printf("Socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 

	// Assign IP and port
	memset(&servaddr, 0, sizeof(servaddr)); 
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
	closesocket(client_socket); 
} 
