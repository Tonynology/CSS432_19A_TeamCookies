#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <time.h>         // gettimeofday
#include <sys/time.h>     // gettimeofday  

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

int main (int argc, char* argv[])
{
    // Initializes passed in command line arguments
    int port = atoi(argv[1]);
    char* serverName = argv[2];

    int sockfd;  
    sockaddr_in server;
    
    struct hostent* host = gethostbyname(serverName);

    bzero((char*)&server, sizeof(server));
    server.sin_family      = AF_INET; // Address Family Internet
    server.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    server.sin_port        = htons(port);

    // Open a new socket and establish the following sequence of code
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0)
    {
        perror("Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (sockaddr*) &server, sizeof(server)) < 0) {
        perror("Connection failed.\n");
        exit(EXIT_FAILURE);
    }

    std::cout << "client port: " << port << std::endl;
    int portChar = htonl(port);

    char* tosend = (char*) &port;

    send(sockfd, tosend, sizeof(port), 0);
    printf("port send to lobby-server\n");

    close(sockfd);
    return 0;
}