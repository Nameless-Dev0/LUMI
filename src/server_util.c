#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "server_util.h"

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
    free(server_address);
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

void set_client_nickname(client_t* client, const char* nickname) {
    strncpy(client->nickname, nickname, MAX_NICK_LEN - 1);
    client->nickname[MAX_NICK_LEN - 1] = '\0';
    client->nickname_set = 1;
    printf("Client nickname set to: '%s'\n", client->nickname);
}

client_t* find_client_by_nickname(const char* nickname, client_t** client_list, int client_count) {
    for(int i = 0; i < client_count; i++) {
        if(client_list[i] != NULL &&
            client_list[i]->nickname_set &&
            strcmp(client_list[i]->nickname, nickname) == 0
        ){return client_list[i];}
    }
    return NULL;
}

void route_message(client_t* sender, char* raw_data, client_t** client_list, int client_count) {
    message_packet_t packet;
    memcpy(&packet, raw_data, sizeof(message_packet_t));

    packet.sender_nickname[MAX_NICK_LEN - 1] = '\0';
    packet.target_nickname[MAX_NICK_LEN - 1] = '\0';
    packet.message[MAX_MSG_LEN - 1] = '\0';

    if(!sender->nickname_set)
        set_client_nickname(sender, packet.sender_nickname);

    printf("Routing message from '%s' to '%s': %s\n", packet.sender_nickname, packet.target_nickname, packet.message);

    client_t* target = find_client_by_nickname(packet.target_nickname, client_list, client_count);
    if(target == NULL) {
        printf("Target client '%s' not found\n", packet.target_nickname);
        const char* error_msg = "Error: Target user not found";
        send(sender->sock_fd, error_msg, strlen(error_msg), 0);
        return;
    }

    char formatted_msg[MAX_MSG_LEN + MAX_NICK_LEN + 10];
    snprintf(formatted_msg, sizeof(formatted_msg), "[%s]: %s", packet.sender_nickname, packet.message);

    ssize_t sent = send(target->sock_fd, formatted_msg, strlen(formatted_msg), 0);
    if(sent < 0)
        perror("Failed to send message to target");
    else
        printf("Message delivered to '%s'\n", packet.target_nickname);
}