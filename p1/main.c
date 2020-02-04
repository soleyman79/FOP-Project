#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "cJSON.c"
#include "cJSON.h"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
#define FULL 100000

int client_socket, server_socket;
struct sockaddr_in servaddr, cli;
char token[MAX], channel[MAX];
bool login = false, current_channel = false;
int status = 0;

/*
void exit_Function(){
    if (login){
        char buffer[MAX];
        sprintf (buffer, "logout %s\n", token);
        memset(token, 0, sizeof(token));
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        closesocket (client_socket);
        cJSON * recived = cJSON_Parse (buffer);
        if (!strncmp("Successful", cJSON_GetObjectItem(recived, "type")->valuestring, sizeof("Successful"))){
            status = 0;
            printf ("You logged out!\n");
            login = false;
        }
        else{
            printf ("Error in logout!\n");
        }
    }
}
*/

void color (int n){
    HANDLE hStdOut;
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, n);
}

void menu1 (){
    color(10);
    printf ("\nMenu1\n1: Register\n2: Login\n");
    int in;
    char username[MAX], password[MAX], buffer[MAX];
    scanf ("%d", &in);
    //register or login
    if (in == 1 || in == 2){
        printf ("Enter your User name\n");
        scanf ("%s", username);
        printf("Enter your password\n");
        scanf ("%s", password);
        //register
        if (in == 1)
            sprintf (buffer, "register %s, %s\n", username, password);
        //login
        if (in == 2)
            sprintf (buffer, "login %s, %s\n", username, password);
        if (in == 2 && login){
            char buffer[MAX];
            sprintf (buffer, "logout %s\n", token);
            make_connect();
            send (client_socket, buffer, sizeof (buffer), 0);
            closesocket (client_socket);
        }
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        cJSON * recived = cJSON_Parse (buffer);
        // Clear the screen
        system("cls");
        //if login is successful
        if (strcmp("AuthToken", cJSON_GetObjectItem (recived, "type") -> valuestring))
            printf ("From server: %s %s\n", cJSON_GetObjectItem (recived, "type") -> valuestring, cJSON_GetObjectItem (recived, "content") -> valuestring);
        //if login is not successful
        else{
            status = 1;
            strcpy (token, cJSON_GetObjectItem (recived, "content") -> valuestring);
            printf ("You logged in successfully\n");
            login = true;
        }
        closesocket(client_socket);
    }
    //unvalued input
    if (!(in == 1 || in == 2)){
        printf ("Wrong input\n");
    }
}

void menu2 (){
    color(9);
    printf ("\nMenu2\n1: Create channel\n2: Join channel\n3: Logout and back to Menu 1\n4: My current channel\n");
    int in;
    scanf ("%d", &in);
    // Clear the screen
    system("cls");
    //if you are in a channel but you want to join or create a channel
    if ((in == 1 || in == 2) && current_channel){
        printf ("You are in another channel! check your current channel\n");
    }
    //create channel
    if (in == 1 && !current_channel){
        printf ("Enter your channel name\n");
        scanf ("%s", channel);
        char buffer[MAX];
        sprintf (buffer, "create channel %s, %s\n", channel, token);
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        cJSON * recived = cJSON_Parse (buffer);
        printf ("From server: %s %s\n", cJSON_GetObjectItem (recived, "type") -> valuestring, cJSON_GetObjectItem (recived, "content") -> valuestring);
        if (!strcmp ("Successful", cJSON_GetObjectItem (recived, "type") -> valuestring)){
            status = 2;
            current_channel = true;
        }
        closesocket (client_socket);
    }
    //join channel
    if (in == 2 && !current_channel){
        printf ("Enter your channel name\n");
        scanf ("%s", channel);
        char buffer[MAX];
        sprintf (buffer, "join channel %s, %s\n", channel, token);
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        cJSON * recived = cJSON_Parse (buffer);
        printf ("From server: %s %s\n", cJSON_GetObjectItem (recived, "type") -> valuestring, cJSON_GetObjectItem (recived, "content") -> valuestring);
        if (!strcmp ("Successful", cJSON_GetObjectItem (recived, "type") -> valuestring)){
            status = 2;
            current_channel = true;
        }
        closesocket (client_socket);
    }
    //logout
    if (in == 3){
        char buffer[MAX];
        sprintf (buffer, "logout %s\n", token);
        memset(token, 0, sizeof(token));
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        closesocket (client_socket);
        cJSON * recived = cJSON_Parse (buffer);
        if (!strncmp("Successful", cJSON_GetObjectItem (recived, "type") -> valuestring, sizeof ("Successful"))){
            status = 0;
            printf ("You logged out!\n");
            login = false;
        }
        else{
            printf ("Error in logout!\n");
        }
    }
    //My current channel
    if (in == 4){
        if (current_channel)
            status = 2;
        else
            printf ("You are not in any channel now!\n");
    }
}

void menu3 (){
    color(14);
    printf ("\nMenu3\nChannel name: %s\n1: Send massage\n2: Refresh\n3: Channel Members\n4: Leave channel\n5: Back to Menu 2\n", channel);
    int in;
    scanf ("%d", &in);
    // Clear the screen
    system("cls");
    //send massage
    if (in == 1){
        printf ("Enter your massage: ");
        char massage[MAX];
        getchar ();
        scanf ("%[^\n]s", massage);
        char buffer[MAX];
        sprintf (buffer, "send %s, %s\n", massage, token);
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        cJSON * recived = cJSON_Parse (buffer);
        printf ("From server: %s %s\n", cJSON_GetObjectItem (recived, "type") -> valuestring, cJSON_GetObjectItem (recived, "content") -> valuestring);
        closesocket (client_socket);
    }
    //refresh
    if (in == 2){
        char buffer[FULL];
        sprintf (buffer, "refresh %s\n", token);
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        cJSON * recived = cJSON_Parse (buffer);
        cJSON * refresh_array = cJSON_GetObjectItem (recived, "content");
        int massage_number = cJSON_GetArraySize (refresh_array);
        printf ("%d updated massages\n", massage_number);
        for (int i = 0; i < massage_number; i++){
            cJSON * refresh_array_item = cJSON_GetArrayItem (refresh_array, i);
            printf ("%s:\n", cJSON_GetObjectItem (refresh_array_item, "sender") -> valuestring);
            printf ("\t%s\n", cJSON_GetObjectItem (refresh_array_item, "content") -> valuestring);
        }
        closesocket (client_socket);
    }
    //channel members
    if (in == 3){
        char buffer[MAX];
        sprintf (buffer, "channel members %s\n", token);
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        cJSON * recived = cJSON_Parse (buffer);
        int member_number =  cJSON_GetArraySize (cJSON_GetObjectItem (recived, "content"));
        printf ("Channel members: %d subscribers\n", member_number);
        for (int i = 0; i < member_number; i++){
            printf ("%s ", cJSON_GetArrayItem (cJSON_GetObjectItem (recived, "content"), i) -> valuestring);
        }
        closesocket (client_socket);
    }
    //leave channel
    if (in == 4){
        char buffer[MAX];
        sprintf (buffer, "leave %s\n", token);
        make_connect();
        send (client_socket, buffer, sizeof (buffer), 0);
        memset (buffer, 0, sizeof (buffer));
        recv (client_socket, buffer, sizeof (buffer), 0);
        cJSON * recived = cJSON_Parse (buffer);
        printf ("From server: %s\n", cJSON_GetObjectItem (recived, "type") -> valuestring);
        closesocket (client_socket);
        status = 1;
        current_channel = false;
    }
    //Back to Menu2
    if (in == 5){
        status = 1;
    }
}

void make_connect(){
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
	/*
	else
		printf("Socket successfully created..\n");
    */

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
	/*
	else
		printf("Successfully connected to the server..\n");
    */
}

int main (){
    printf ("Hello! Welcome\n");
    while (true){
        //atexit(exit_Function);
        if (status == 0)
            menu1 ();
        if (status == 1)
            menu2 ();
        if (status == 2)
            menu3 ();
    }
    //final logout
    char buffer[MAX];
    sprintf (buffer, "logout %s\n", token);
    make_connect();
    send (client_socket, buffer, sizeof (buffer), 0);
    closesocket (client_socket);
    status = 0;
}
