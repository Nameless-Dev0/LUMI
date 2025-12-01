#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "ServerUtils.h"

int SetupServer(char *ip, int port){
    //Socket Creation
    int ServerSocketFD = socket(AF_INET, SOCK_STREAM, 0);
    //Address Creation
    struct sockaddr_in *SocketAddress = malloc(sizeof(struct sockaddr_in));
    SocketAddress->sin_port = htons(port);
    SocketAddress->sin_family = AF_INET;
    
    if(strlen(ip) == 0)

        SocketAddress->sin_addr.s_addr = INADDR_ANY;
    else
        inet_pton(AF_INET,ip,(struct sockaddr*)&SocketAddress->sin_addr.s_addr);

    struct sockaddr_in *ServerAddress = SocketAddress;

    //Binding and Validation
    if(bind(ServerSocketFD, (struct sockaddr*)ServerAddress, sizeof(*ServerAddress)) == 0)
        printf("Socket bond to server successfully!\n");

    //Listening and Validation
    if(listen(ServerSocketFD, 10) == 0)
        printf("Server is Listening!\n");


    return ServerSocketFD;

}


struct AcceptedSocket* AcceptIncomingConnection(int ServerSocketFD){
    struct sockaddr_in *ClientAddress = malloc(sizeof(struct sockaddr_in)); 
    int AddressSize = sizeof(struct sockaddr_in );
    int ClientSocketFD = accept(ServerSocketFD, (struct sockaddr*)ClientAddress, &AddressSize);

    struct AcceptedSocket*  socketaccepted = malloc(sizeof (struct AcceptedSocket));
    socketaccepted->AcceptedSocketFD = ClientSocketFD;
    socketaccepted->address = *ClientAddress;

    if(ClientSocketFD < 0){
        socketaccepted->error = ClientSocketFD;
        printf("Socket Acceptance Falied!\n");
    }
    else
        printf("Client Connected:\n");

    return socketaccepted;
}



void ReceiveAndPrint(struct AcceptedSocket* ClientSocket){
    //Receiving and Printing
    char buffer[1024];
    while(true)
    {
            size_t AmountReceived =  recv(ClientSocket->AcceptedSocketFD, buffer, 1024, 0);
            if(AmountReceived > 0){

                buffer[AmountReceived] = 0;
                printf("Response was %s\n ", buffer);
            }
            if(AmountReceived == 0)
                break;
    }   
}
