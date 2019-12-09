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
#define DEFAULT_SERVERSD -1
#define DEFAULT_NEWSD -1

int port = DEFAULT_PORT;
int serverSd = DEFAULT_SERVERSD;
int newSd = DEFAULT_NEWSD;

void *server(void *)
{
    int sd = newSd;
    while (true) {
        Etc::consoleOut("message: ");
        
	    std::string message = Etc::portIn(sd); /// three-way handshake
	    Etc::portOut(sd, message);
	    std::string acknowledgement = Etc::portIn(sd);
        
        Etc::consoleOut(message + "\nacknowledgement: " + acknowledgement + "\n");
        Etc::errChk(-1 + (message == acknowledgement), "Something has gone terribly wrong!");
    }
    close(serverSd);
	close(sd);
}

int main(int argc, char const *argv[])
{	
    Etc::serverSetup(port, serverSd);
    while (true) {
        sockaddr_in newSockAddr;
        newSd = Etc::portAccept(serverSd, newSockAddr);
        
	    pthread_t t;
        //pthread_attr_t attr;
        //pthread_attr_init(&attr);
        
	    Etc::errChk(pthread_create(&t, NULL, server, NULL ), "Error: Thread failed to create.");
	    Etc::errChk(pthread_detach(t), "Error: Thread failed to detach.");
        //Etc::errChk(pthread_join(t, NULL), "Error: Thread failed to join.");
        //pthread_cancel(t);
        //pthread_exit(NULL)
    }
    exit(0);
    return 0;
}
