#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "ServerUtils.h"


int main() {

    int ServerSocketFD = SetupServer("127.0.0.1", 1500);

    struct AcceptedSocket* ClientSocket = AcceptIncomingConnection(ServerSocketFD);

    ReceiveAndPrint(ClientSocket);

    close(ClientSocket->AcceptedSocketFD);
    shutdown(ServerSocketFD, SHUT_RDWR);


    return 0;
}

