#include <netinet/in.h>
#define MAX_NICK_LEN 32

typedef struct client {
    int sock_fd;
    struct sockaddr_in address;
    char nickname[MAX_NICK_LEN];
    int error;
} client_t;

int setup_server(char *ip, int port);
client_t* accept_incoming_connection(int server_socket_fd);
void receive_and_print(client_t* client_socket);