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
    //Socket Creation
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //Address Creation
    struct sockaddr_in *socket_address = malloc(sizeof(struct sockaddr_in));
    socket_address->sin_port = htons(port);
    socket_address->sin_family = AF_INET;
    
    if(strlen(ip) == 0)
        socket_address->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET, ip, (struct sockaddr*)&socket_address->sin_addr.s_addr);
    struct sockaddr_in *server_address = socket_address;
    //Binding and Validation
    if(bind(server_socket_fd, (struct sockaddr*)server_address, sizeof(*server_address)) == 0)
        printf("Socket bond to server successfully!\n");
    //Listening and Validation
    if(listen(server_socket_fd, 10) == 0)
        printf("Server is Listening!\n");
    return server_socket_fd;
}

struct accepted_socket* accept_incoming_connection(int server_socket_fd){
    struct sockaddr_in *client_address = malloc(sizeof(struct sockaddr_in)); 
    int address_size = sizeof(struct sockaddr_in);
    int client_socket_fd = accept(server_socket_fd, (struct sockaddr*)client_address, &address_size);
    struct accepted_socket* socket_accepted = malloc(sizeof(struct accepted_socket));
    socket_accepted->accepted_socket_fd = client_socket_fd;
    socket_accepted->address = *client_address;
    if(client_socket_fd < 0){
        socket_accepted->error = client_socket_fd;
        printf("Socket Acceptance Falied!\n");
    }
    else
        printf("Client Connected:\n");
    return socket_accepted;
}

void receive_and_print(struct accepted_socket* client_socket){
    //Receiving and Printing
    char buffer[1024];
    while(true)
    {
            size_t amount_received = recv(client_socket->accepted_socket_fd, buffer, 1024, 0);
            if(amount_received > 0){
                buffer[amount_received] = 0;
                printf("Response was %s\n ", buffer);
            }
            if(amount_received == 0)
                break;
    }   
}