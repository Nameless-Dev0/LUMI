#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include "server_util.h"

int main() {
    int server_socket_fd = setup_server("127.0.0.1", 1500);
    while(true){
        client_t* incoming_client = accept_incoming_connection(server_socket_fd);
        receive_and_print(incoming_client);
        free(incoming_client);
    }
    
    shutdown(server_socket_fd, SHUT_RDWR);
    return 0;
}