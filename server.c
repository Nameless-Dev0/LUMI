#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "ServerUtils.h"
#include <stdbool.h>


int main() {

    int ServerSocketFD = SetupServer("127.0.0.1", 1500);

    while(true){
    struct AcceptedSocket* ClientSocket = AcceptIncomingConnection(ServerSocketFD);

    ReceiveAndPrint(ClientSocket);
    }
    
    shutdown(ServerSocketFD, SHUT_RDWR);


    return 0;
}

