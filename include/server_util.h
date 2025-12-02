#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H
#include <netinet/in.h>

#define MAX_NICK_LEN 32
#define MAX_CLIENTS 1024
#define MAX_MSG_LEN 256

// Message structure that matches Python client's packed format
typedef struct {
    char sender_nickname[MAX_NICK_LEN];
    char target_nickname[MAX_NICK_LEN];
    char message[MAX_MSG_LEN];
} message_packet_t;

typedef struct client {
    int sock_fd;
    struct sockaddr_in address;
    char nickname[MAX_NICK_LEN];
    int error;
    int nickname_set;  // Flag to track if nickname has been set
} client_t;

int setup_server(char *ip, int port);
client_t* accept_incoming_connection(int server_socket_fd);
void receive_and_print(client_t* client_socket);
void route_message(client_t* sender, char* raw_data, client_t** client_list, int client_count);
client_t* find_client_by_nickname(const char* nickname, client_t** client_list, int client_count);
void set_client_nickname(client_t* client, const char* nickname);

#endif //SERVER_UTIL_H