#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1500);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return 1;
    }

    printf("Connected to server!\n");

    char buffer[1024];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(buffer, sizeof(buffer), stdin))
            break;

        // remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        if (send(client_fd, buffer, strlen(buffer), 0) < 0) {
            perror("send");
            break;
        }
    }

    close(client_fd);
    return 0;
}