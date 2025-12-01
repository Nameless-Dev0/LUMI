#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "server_util.h"

static client_t* client_list[MAX_CLIENTS];

int setup_server(char *ip, int port){
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in *server_address = malloc(sizeof(struct sockaddr_in));
    server_address->sin_port = htons(port);
    server_address->sin_family = AF_INET;

    if(strlen(ip) == 0)
        server_address->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET, ip, &server_address->sin_addr.s_addr);

    if(bind(server_socket_fd, (struct sockaddr*)server_address, sizeof(*server_address)) == 0)
        printf("Socket bound to server successfully!\n");

    if(listen(server_socket_fd, 10) == 0)
        printf("Server is Listening!\n");

    return server_socket_fd;
}

client_t* accept_incoming_connection(int server_socket_fd){
    struct sockaddr_in client_address;
    socklen_t address_size = sizeof(struct sockaddr_in);
    int accepted_client_fd = accept(server_socket_fd, (struct sockaddr*)&client_address, &address_size);
    client_t* accepted_client = malloc(sizeof(client_t));

    accepted_client->sock_fd = accepted_client_fd;
    accepted_client->address = client_address;

    if(accepted_client_fd < 0){
        accepted_client->error = accepted_client_fd;
        printf("Socket Acceptance Failed!\n");
    }
    else
        printf("Client Connected:\n");

    return accepted_client;
}

void receive_and_print(client_t* client){
    char buffer[1024];
    ssize_t amount_received;

    while(1){
        amount_received = recv(client->sock_fd, buffer, sizeof(buffer) - 1, 0);
        if(amount_received == -1){
            perror("receive failed!");
            break;
        }
        if(amount_received == 0){
            printf("client disconnected!\n");
            break;
        }

        buffer[amount_received] = '\0';
        printf("Response: %s\n", buffer);

    }
}