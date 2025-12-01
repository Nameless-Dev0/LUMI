#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "server_util.h"
#include <stdbool.h>

int main() {
    int server_socket_fd = setup_server("127.0.0.1", 1500);
    while(true){
        struct accepted_socket* client_socket = accept_incoming_connection(server_socket_fd);
        receive_and_print(client_socket);
    }
    
    shutdown(server_socket_fd, SHUT_RDWR);
    return 0;
}