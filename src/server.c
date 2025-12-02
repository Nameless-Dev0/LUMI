#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include "server_util.h"
#include <fcntl.h>
#include <sys/select.h>

static client_t* client_list[MAX_CLIENTS];
static int client_count = 0;

static void set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl return fail");
        return;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl set fail");
        return;
    }
}

int main() {
    int server_socket_fd = setup_server("127.0.0.1", 1500);
    set_non_blocking(server_socket_fd);
    
    fd_set read_fds, all_fds;
    FD_ZERO(&all_fds);
    FD_SET(server_socket_fd, &all_fds);
    int max_fd = server_socket_fd;
    
    while(true) {
        read_fds = all_fds;
        
        if(select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select fail");
            continue;
        }

        if(FD_ISSET(server_socket_fd, &read_fds)) {
            client_t* incoming_client = accept_incoming_connection(server_socket_fd);
            
            if(incoming_client == NULL || incoming_client->sock_fd < 0) {
                perror("accept fail");
                free(incoming_client);
            }
            else if(client_count < MAX_CLIENTS) {
                set_non_blocking(incoming_client->sock_fd);
                memset(incoming_client->nickname, 0, MAX_NICK_LEN);
                incoming_client->nickname_set = 0;

                client_list[client_count] = incoming_client;
                client_count++;

                FD_SET(incoming_client->sock_fd, &all_fds);
                if(incoming_client->sock_fd > max_fd) {
                    max_fd = incoming_client->sock_fd;
                }
                
                printf("Client added. Total clients: %d\n", client_count);
            }
            else {
                printf("Maximum clients reached\n");
                close(incoming_client->sock_fd);
                free(incoming_client);
            }
        }
        
        for (int i = 0; i < client_count; i++) {
            if (client_list[i] == NULL)
                continue;

            int fd = client_list[i]->sock_fd;
            if (!FD_ISSET(fd, &read_fds))
                continue;

            char buffer[1024];
            ssize_t amount_received = recv(fd, buffer, sizeof(buffer), 0);

            if (amount_received > 0) {
                route_message(client_list[i], buffer, client_list, client_count);
                continue;
            }

            if (amount_received == 0) {
                if (client_list[i]->nickname_set)
                    printf("Client '%s' disconnected\n", client_list[i]->nickname);
                else
                    printf("Client (no nickname) disconnected\n");

                FD_CLR(fd, &all_fds);
                close(fd);
                free(client_list[i]);

                for (int j = i; j < client_count - 1; j++)
                    client_list[j] = client_list[j + 1];

                client_count--;
                i--;
                continue;
            }

            if (errno != EAGAIN && errno != EWOULDBLOCK)
                perror("recv error");
        }
    }

    for(int i = 0; i < client_count; i++) {
        if(client_list[i] != NULL) {
            close(client_list[i]->sock_fd);
            free(client_list[i]);
        }
    }
    
    shutdown(server_socket_fd, SHUT_RDWR);
    return 0;
}