#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <regex.h>
#include "cJSON.c"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

int grid[3][3];

regex_t regex;

int print_grid()
{
    puts("");
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            if (!grid[i][j])
                printf(".");
            else if (grid[i][j] == 1)
                printf("X");
            else
                printf("O");
        puts("");
    }
    puts("");

    // Determine the winner
    for (int i = 0; i < 3; i++)
    {
        if (grid[i][0] == grid[i][1] && grid[i][1] == grid[i][2] && grid[i][0] != 0)
            return grid[i][0];
        if (grid[0][i] == grid[1][i] && grid[1][i] == grid[2][i] && grid[0][i] != 0)
            return grid[0][i];
    }
    if (grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2] && grid[1][1] != 0)
        return grid[1][1];
    if (grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0] && grid[1][1] != 0)
        return grid[1][1];
    int sum = 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            sum += grid[i][j];
    if (sum == 13)
        return -1;
    return 0;
}

bool game_ended()
{
    int winner = print_grid();
    if (winner == 1)
    {
        puts("You win.");
        return true;
    }
    else if (winner == -1)
    {
        puts("Draw");
        return true;
    }
    else if (winner == 2)
    {
        puts("You lose.");
        return true;
    }
    return false;
}

void play(int server_socket)
{
    int n, match, x, y;
    char buffer[MAX], *message;
    cJSON *coordinates;
    puts("Start!");
    while (true)
    {
        bzero(buffer, sizeof(buffer));
        while (true)
        {

            // Copy client message to the buffer
            n = 0;
            while ((buffer[n++] = getchar()) != '\n')
                ;
            if (strncmp("exit", buffer, 4) == 0)
            {
                send(server_socket, buffer, sizeof(buffer), 0);
                printf("Client stopping...\n");
                return;
            }
            match = regexec(&regex, buffer, 0, NULL, 0);
            if (!match)
            {
                x = buffer[0] - '1';
                y = buffer[2] - '1';
                if (!grid[x][y])
                    break;
            }
        }
        grid[x][y] = 1;

        // Create the message to transmit coordinates
        coordinates = cJSON_CreateObject();
        cJSON_AddNumberToObject(coordinates, "x", x);
        cJSON_AddNumberToObject(coordinates, "y", y);
        message = cJSON_Print(coordinates);

        // Send the message to server
        send(server_socket, message, strlen(message), 0);
        cJSON_Delete(coordinates);

        // See if game has ended
        if (game_ended())
            return;
        bzero(buffer, sizeof(buffer));

        // Read the message from server and copy it to buffer
        recv(server_socket, buffer, sizeof(buffer), 0);

        // If the message starts with "exit" then client exits and chat ends
        if ((strncmp(buffer, "exit", 4)) == 0)
        {
            printf("Client stopping...\n");
            return;
        }

        // Interpret the message containing the coordinates
        coordinates = cJSON_Parse(buffer);
        x = cJSON_GetObjectItemCaseSensitive(coordinates, "x")->valueint;
        y = cJSON_GetObjectItemCaseSensitive(coordinates, "y")->valueint;
        grid[x][y] = 2;
        cJSON_Delete(coordinates);
        // See if game has ended
        if (game_ended())
            return;
    }
}

int main()
{
    regcomp(&regex, "[1-3],[1-3]", REG_EXTENDED);

    int client_socket, server_socket;
    struct sockaddr_in servaddr, cli;

    // Create and verify socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
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
    if (connect(client_socket, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Connection to the server failed...\n");
        exit(0);
    }
    else
        printf("Successfully connected to the server..\n");

    // Function for chat
    play(client_socket);

    // Close the socket
    shutdown(client_socket, SHUT_RDWR);
}