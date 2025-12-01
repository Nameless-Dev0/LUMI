#include <netinet/in.h>

struct accepted_socket {
    int accepted_socket_fd;
    struct sockaddr_in address;
    int error;
};

int setup_server(char *ip, int port);
struct accepted_socket* accept_incoming_connection(int server_socket_fd);
void receive_and_print(struct accepted_socket* client_socket);