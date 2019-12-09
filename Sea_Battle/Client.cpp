#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

#include <string>
#include <iostream>
#include <future>

#include "Etc.h"

#define DEFAULT_PORT 6932
#define DEFAULT_ADDRESS "uw1-320-12"
#define DEFAULT_CLIENTSD -1

int port = DEFAULT_PORT;
std::string address = DEFAULT_ADDRESS;
int clientSd = DEFAULT_CLIENTSD;
sockaddr_in sendSockAddr;

int main(int argc, char const *argv[])
{	
    Etc::clientSetup(port, address, clientSd, sendSockAddr);
    while (true) {
        Etc::portConnect(clientSd, sendSockAddr);

        Etc::consoleOut("message: ");
        std::string message = Etc::consoleIn();

	    Etc::portOut(clientSd, message); /// three-way handshake
	    std::string acknowledgement = Etc::portIn(clientSd);
	    Etc::portOut(clientSd, acknowledgement);

        Etc::consoleOut("acknowledgement: " + acknowledgement + "\n");
        Etc::errChk(-1 + (message == acknowledgement), "Something has gone terribly wrong!");
    }
	close(clientSd);
	exit(0);
    return 0;
}
