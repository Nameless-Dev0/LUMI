#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

#include <netinet/in.h>

#define MAX_NICK_LEN 32
#define MAX_CLIENTS 1024

typedef struct client {
    int sock_fd;
    struct sockaddr_in address;
    char nickname[MAX_NICK_LEN];
    int error;
} client_t;

int setup_server(char *ip, int port);
client_t* accept_incoming_connection(int server_socket_fd);
void receive_and_print(client_t* client_socket);
void route_message(client_t* client_socket, char* message); // recv raw bytes (packed struct) -> parse & extract target nick ->

#endif //SERVER_UTIL_H