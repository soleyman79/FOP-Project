#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <time.h>
#include "cJSON.h"
#include "cJSON.c"
#include <conio.h>
#define MAX 200000
#define MIN 200
#define PORT 12345
#define SA struct sockaddr
#define TokenSize 32
#define ServerSize 200

struct mystruct{
    char onlineUser[MIN];
    char currenChannel[MIN];
    char token[TokenSize];
    int refreshNo;
    bool inChannel;
};

//Function prototype
void register_Function();
void login_Function();
void create_channel_Function();
void join_channel_Function();
void removing_space(char*);
void send_Function();
void refresh_Function();
void channel_members_Function();
void leave_Function();
void logout_Function();
void exit_Function();
int checkToken();

struct mystruct users[ServerSize];
cJSON* root;
int server_socket, client_socket;
int err;
int i;
int count = 0;
struct sockaddr_in server, client;
char buffer[MAX];
WORD wVersionRequested;
WSADATA wsaData;

void exit_Function(){
    for (int j = 0; j < count; j++){
        if (users[j].inChannel){
            leave_Function();
        }
        logout_Function();
    }
}

int checkToken(char authToken[TokenSize]){
    for (i = 0; i < count; i++){
        if (!strncmp(authToken, users[i].token, TokenSize)){
            return i;
        }
    }
    return -1;
}

void removing_space(char *str1) {
    char* str2 = str1;
    while(*str1 != '\0') {
        if(*str1 != '\t' && *str1 != '\n') {
            *str2++ = *str1++;
        } else {
            ++str1;
        }
    }
    *str2 = '\0';
}

char* randstring (int length) {
    int mySeed = time (NULL);
    char *string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
    size_t stringLen = strlen(string);
    char* randomString = NULL;

    srand(time(NULL) * length + ++mySeed);

    if (length < 1) {
        length = 1;
    }

    randomString = malloc(sizeof(char) * (length +1));

    if (randomString) {
        short key = 0;

        for (int n = 0;n < length;n++) {
            key = rand() % stringLen;
            randomString[n] = string[key];
        }

        randomString[length] = '\0';

        return randomString;
    }
    else {
        printf("No memory");
        exit(1);
    }
}

void register_Function(){
    char username[MIN], password[MIN], path[MAX];
    sscanf (buffer, "register %[^,],%s", username, password);
    sprintf (path, "Resources\\Users\\%s.user.json", username);
    FILE* file = fopen (path, "r");
    if (file != NULL){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("This name is not available"));
    }
    else{
        fclose(file);
        file = fopen (path, "w");
        fprintf (file, "{\"username\":\"%s\",\"password\":\"%s\"}", username, password);
        // Make the response to client
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Successful"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("You registered successfully"));

        fclose (file);
    }
}

void login_Function(){
    char username[MIN], password[MIN], path[MAX];
    sscanf (buffer, "login %[^,],%s", username, password);
    sprintf (path, "Resources\\Users\\%s.user.json", username);
    FILE* file = fopen (path, "r");
    if (file == NULL){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("This name is not existing"));
    }
    else {
        char pass[MIN], str[MAX];
        int n = 0;
        while (!feof (file)){
            str[n++] = fgetc (file);
        }
        cJSON* json = cJSON_Parse (str);
        strcpy (pass, cJSON_GetObjectItem (json, "password") -> valuestring);
        if (strcmp(pass, password)){
            root = cJSON_CreateObject();
            cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
            cJSON_AddItemToObject(root, "content", cJSON_CreateString("Wrong password"));
        }
        else{
            strcpy (users[count].token, randstring (TokenSize));
            strcpy (users[count].onlineUser, username);
            root = cJSON_CreateObject();
            cJSON_AddItemToObject(root, "type", cJSON_CreateString("AuthToken"));
            cJSON_AddItemToObject(root, "content", cJSON_CreateString(users[count].token));
            count++;
        }
        fclose(file);
    }
}

void create_channel_Function(){
    char authToken[TokenSize], channelName[MIN];
    sscanf (buffer, "create channel %[^,],%s", channelName, authToken);
    if (checkToken(authToken) == -1){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("Wrong token"));
        return;
    }
    char path[MAX];
    sprintf (path, "Resources\\Channels\\%s.channel.json", channelName);
    FILE* file = fopen (path, "r");
    if (file != NULL){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("This name is not available"));
        fclose(file);
    }
    else {
        // Make the response to client
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Successful"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString(""));

        cJSON* json = cJSON_CreateObject();
        cJSON* messageArray = cJSON_CreateArray();
        cJSON* message = cJSON_CreateObject();
        cJSON_AddItemToObject(message, "sender", cJSON_CreateString("server"));
        char str1[MIN];
        sprintf(str1, "%s created %s", users[i].onlineUser, channelName);
        cJSON_AddItemToObject(message, "content", cJSON_CreateString(str1));
        cJSON_AddItemToArray(messageArray, message);
        cJSON_AddItemToObject(json, "messages", messageArray);
        cJSON_AddItemToObject(json, "name", cJSON_CreateString(channelName));

        // Add member to channel database
        cJSON* members = cJSON_CreateArray();
        cJSON_AddItemToArray(members, cJSON_CreateString(users[i].onlineUser));
        cJSON_AddItemToObject(json, "subscribers", members);

        char str2[MAX];
        strcpy(str2, cJSON_Print(json));
        file = fopen (path, "w");
        removing_space(str2);
        fprintf(file, str2);
        fclose(file);

        users[i].inChannel = true;
        strcpy(users[i].currenChannel, channelName);
        users[i].refreshNo = 0;
    }
}

void join_channel_Function(){
    char authToken[TokenSize], channelName[MIN];
    sscanf(buffer, "join channel %[^,],%s", channelName, authToken);
    if (checkToken(authToken) == -1){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("Wrong token"));
        return;
    }
    if (users[i].inChannel){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("You are in another channel"));
        return;
    }
    char path[MAX];
    sprintf (path, "Resources\\Channels\\%s.channel.json", channelName);
    FILE* file = fopen (path, "r");
    if (file == NULL){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("This channel is not existing"));
    }
    else{
        users[i].inChannel = true;
        users[i].refreshNo = 0;
        memset(users[i].currenChannel, 0, sizeof(users[i].currenChannel));
        strcpy(users[i].currenChannel, channelName);
        char str1[MAX], str2[MAX];
        int n = 0;
        while (!feof (file)){
            str1[n++] = fgetc (file);
        }
        fclose(file);
        cJSON* json = cJSON_Parse(str1);
        cJSON* Message = cJSON_CreateObject();
        cJSON_AddItemToObject(Message, "sender", cJSON_CreateString("server"));
        sprintf(str2, "%s joined.", users[i].onlineUser);
        cJSON_AddItemToObject(Message, "content", cJSON_CreateString(str2));
        cJSON_AddItemToArray(cJSON_GetObjectItem(json, "messages"), Message);

        // Add member to channel database
        cJSON_AddItemToArray(cJSON_GetObjectItem(json, "subscribers"), cJSON_CreateString(users[i].onlineUser));

        memset(str1, 0, sizeof(str1));
        strcpy(str1, cJSON_Print(json));
        file = fopen(path, "w");
        removing_space(str1);
        fprintf(file, str1);
        fclose(file);
        // Make the response to client
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Successful"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString(""));
    }
}

void send_Function(){
    char authToken[TokenSize], text[MAX];
    sscanf(buffer, "send %[^,],%s", text, authToken);
    if (checkToken(authToken) == -1){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("Wrong token"));
        return;
    }
    if (!users[i].inChannel){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("You are not in any channel"));
        return;
    }
    char path[MAX];
    sprintf (path, "Resources\\Channels\\%s.channel.json", users[i].currenChannel);
    FILE* file = fopen (path, "r");
    char str1[MAX];
    int n = 0;
    while (!feof (file)){
        str1[n++] = fgetc (file);
    }
    fclose(file);
    cJSON* json = cJSON_Parse(str1);
    cJSON* Message = cJSON_CreateObject();
    cJSON_AddItemToObject(Message, "sender", cJSON_CreateString(users[i].onlineUser));
    cJSON_AddItemToObject(Message, "content", cJSON_CreateString(text));
    cJSON_AddItemToArray(cJSON_GetObjectItem(json, "messages"), Message);
    memset(str1, 0, sizeof(str1));
    strcpy(str1, cJSON_Print(json));
    file = fopen(path, "w");
    removing_space(str1);
    fprintf(file, str1);
    fclose(file);

    // Make the response to client
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "type", cJSON_CreateString("Successful"));
    cJSON_AddItemToObject(root, "content", cJSON_CreateString(""));
}

void refresh_Function(){
    char authToken[TokenSize];
    sscanf(buffer, "refresh %s", authToken);
    if (checkToken(authToken) == -1){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("Wrong token"));
        return;
    }
    if (!users[i].inChannel){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("You are not in any channel"));
        return;
    }
    char path[MAX];
    sprintf (path, "Resources\\Channels\\%s.channel.json", users[i].currenChannel);
    FILE* file = fopen (path, "r");
    char* str1 = (char*) malloc(sizeof(char));
    int n = 0;
    while (!feof (file)){
        str1[n++] = fgetc (file);
        str1 = (char*) realloc(str1, (n+1)*sizeof (char));
    }
    fclose(file);
    cJSON* json = cJSON_Parse(str1);
    cJSON* content = cJSON_CreateArray();
    cJSON_AddItemToArray(content, cJSON_GetArrayItem(cJSON_GetObjectItem(json, "messages"), users[i].refreshNo));
    users[i].refreshNo = cJSON_GetArraySize(content);

    // Make the response to client
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "type", cJSON_CreateString("List"));
    cJSON_AddItemToObject(root, "content", content);
}

void channel_members_Function(){
    char authToken[TokenSize];
    sscanf(buffer, "channel members %s", authToken);
    if (checkToken(authToken) == -1){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("Wrong token"));
        return;
    }
    if (!users[i].inChannel){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("You are not in any channel"));
        return;
    }
    char path[MAX];
    sprintf (path, "Resources\\Channels\\%s.channel.json", users[i].currenChannel);
    FILE* file = fopen (path, "r");
    char str1[MAX];
    int n = 0;
    while (!feof (file)){
        str1[n++] = fgetc (file);
    }
    fclose(file);
    cJSON* json = cJSON_Parse(str1);
    cJSON* subscribers = cJSON_GetObjectItem(json, "subscribers");

    // Make the response to client
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "type", cJSON_CreateString("List"));
    cJSON_AddItemToObject(root, "content", subscribers);
}

void leave_Function(){
    char authToken[TokenSize];
    sscanf(buffer, "leave %s", authToken);
    if (checkToken(authToken) == -1){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("Wrong token"));
        return;
    }
    if (!users[i].inChannel){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("You are not in any channel"));
        return;
    }
    char path[MAX];
    sprintf (path, "Resources\\Channels\\%s.channel.json", users[i].currenChannel);
    FILE* file = fopen (path, "r");
    char str1[MAX], str2[MAX];
    int n = 0;
    while (!feof (file)){
        str1[n++] = fgetc (file);
    }
    fclose(file);
    cJSON* json = cJSON_Parse(str1);
    cJSON* message = cJSON_CreateObject();
    cJSON_AddItemToObject(message, "sender", cJSON_CreateString("server"));
    sprintf(str2, "%s left.", users[i].onlineUser);
    cJSON_AddItemToObject(message, "content", cJSON_CreateString(str2));
    cJSON_AddItemToArray(cJSON_GetObjectItem(json, "messages"), message);

    // Remove member from subscribers
    cJSON* subscribers = cJSON_GetObjectItem(json, "subscribers");
    for (int j = 0; j < cJSON_GetArraySize(subscribers); j++){
        char name[MIN], ONLINEUSER[MIN];
        sprintf(ONLINEUSER, "\"%s\"", users[i].onlineUser);
        strcpy(name, cJSON_Print(cJSON_GetArrayItem(subscribers, j)));
        if(!strcmp(name, ONLINEUSER)){
            cJSON_DeleteItemFromArray(subscribers, j);
            break;
        }
    }
    cJSON_DeleteItemFromObject(json, "subscribers");
    cJSON_AddItemToObject(json, "subscribers", subscribers);

    // Write in database
    memset(str1, 0, sizeof(str1));
    strcpy(str1, cJSON_Print(json));
    file = fopen(path, "w");
    removing_space(str1);
    fprintf(file, str1);
    fclose(file);

    // Make the response to client
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "type", cJSON_CreateString("Successful"));
    cJSON_AddItemToObject(root, "content", cJSON_CreateString(""));

    memset(users[i].currenChannel, 0, sizeof(users[i].currenChannel));
    users[i].inChannel = false;
}

void logout_Function(){
    char authToken[TokenSize];
    sscanf(buffer, "logout %s", authToken);
    if (checkToken(authToken) == -1){
        root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(root, "content", cJSON_CreateString("Wrong token"));
        return;
    }
    if (users[i].inChannel){
        char path[MAX];
        sprintf (path, "Resources\\Channels\\%s.channel.json", users[i].currenChannel);
        FILE* file = fopen (path, "r");
        char str1[MAX], str2[MAX];
        int n = 0;
        while (!feof (file)){
            str1[n++] = fgetc (file);
        }
        fclose(file);
        cJSON* json = cJSON_Parse(str1);
        cJSON* message = cJSON_CreateObject();
        cJSON_AddItemToObject(message, "sender", cJSON_CreateString("server"));
        sprintf(str2, "%s left.", users[i].onlineUser);
        cJSON_AddItemToObject(message, "content", cJSON_CreateString(str2));
        cJSON_AddItemToArray(cJSON_GetObjectItem(json, "messages"), message);

        // Remove member from subscribers
        cJSON* subscribers = cJSON_GetObjectItem(json, "subscribers");
        for (int j = 0; j < cJSON_GetArraySize(subscribers); j++){
            char name[MIN], ONLINEUSER[MIN];
            sprintf(ONLINEUSER, "\"%s\"", users[i].onlineUser);
            strcpy(name, cJSON_Print(cJSON_GetArrayItem(subscribers, j)));
            if(!strcmp(name, ONLINEUSER)){
                cJSON_DeleteItemFromArray(subscribers, j);
                break;
            }
        }
        cJSON_DeleteItemFromObject(json, "subscribers");
        cJSON_AddItemToObject(json, "subscribers", subscribers);

        // Write in database
        memset(str1, 0, sizeof(str1));
        strcpy(str1, cJSON_Print(json));
        file = fopen(path, "w");
        removing_space(str1);
        fprintf(file, str1);
        fclose(file);
    }
    // Removing token
    memset(users[i].token, 0, TokenSize);
    memset(users[i].onlineUser, 0, sizeof(users[i].onlineUser));
    users[i] = users[count];
    count--;
    // Make the response to client
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "type", cJSON_CreateString("Successful"));
    cJSON_AddItemToObject(root, "content", cJSON_CreateString(""));
}

int main(){

    while(true){
        atexit(exit_Function);

        // Use the MAKEWORD(low byte, high byte) macro declared in Windef.h
        wVersionRequested = MAKEWORD(2, 2);

        err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0){
            // Tell the user that we could not find a usable Winsock DLL.
            printf("WSAStartup failed with error: %d\n", err);
            return 1;
        }

        // Create and verify socket
        server_socket = socket(AF_INET, SOCK_STREAM, 6);
        if (server_socket == INVALID_SOCKET)
            wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
        else
            printf("Socket successfully created..\n");

        // Assign IP and port
        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htonl(INADDR_ANY);
        server.sin_port = htons(PORT);

        // Bind newly created socket to given IP and verify
        if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0){
            printf("Socket binding failed...\n");
            exit(0);
        }
        else
            printf("Socket successfully bound..\n");

        // Now server is ready to listen and verify
        if ((listen(server_socket, 5)) != 0){
            printf("Listen failed...\n");
            exit(0);
        }
        else
            printf("Server listening..\n");

        // Accept the data packet from client and verify
        int len = sizeof(client);
        client_socket = accept(server_socket, (SA *)&client, &len);
        if (client_socket < 0)
        {
            printf("Server accceptance failed...\n");
            exit(0);
        }
        else
            printf("Server acccepted the client..\n");

        memset(buffer, 0, sizeof(buffer));

        // Read the message from client and copy it to buffer
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf ("From client : %s\n", buffer);

        // Analyzing client message
        if (!strncmp (buffer, "register", strlen ("register"))){
            register_Function();
        }
        else if (!strncmp (buffer, "login", strlen ("login"))){
            login_Function();
            count++;
        }
        else if (!strncmp (buffer, "create channel", strlen ("create channel"))){
            create_channel_Function();
        }
        else if (!strncmp (buffer, "join channel", strlen ("join channel"))){
            join_channel_Function();
        }
        else if (!strncmp (buffer, "send", strlen ("send"))){
            send_Function();
        }
        else if (!strncmp (buffer, "refresh", strlen ("refresh"))){
            refresh_Function();
        }
        else if (!strncmp (buffer, "channel members", strlen ("channel members"))){
            channel_members_Function();
        }
        else if (!strncmp (buffer, "leave", strlen ("leave"))){
            leave_Function();
        }
        else if (!strncmp (buffer, "logout", strlen ("logout"))){
            logout_Function();
        }

        // Print buffer which contains the client message
        memset(buffer, 0, sizeof(buffer));
        strcpy (buffer, cJSON_Print (root));
        removing_space(buffer);
        printf("To client : %s\n", buffer);

        // Send the buffer to client
        send(client_socket, buffer, sizeof(buffer), 0);

        // If the message starts with "exit" then server exits and chat ends
        if (strncmp("exit", buffer, 4) == 0){
            printf("Server stopping...\n");
        }

        // Close the socket
        closesocket(server_socket);
    }
}
