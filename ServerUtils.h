#include <netinet/in.h>



struct AcceptedSocket{
    int AcceptedSocketFD;
    struct sockaddr_in address;
    int error;
};

int SetupServer(char *ip, int port);
struct AcceptedSocket* AcceptIncomingConnection(int ServerSocketFD);
void ReceiveAndPrint(struct AcceptedSocket* ClientSocket);
