#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {

    int clientFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFD < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1500);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(clientFD, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
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

        if (send(clientFD, buffer, strlen(buffer), 0) < 0) {
            perror("send");
            break;
        }
    }

    close(clientFD);
    return 0;
}
