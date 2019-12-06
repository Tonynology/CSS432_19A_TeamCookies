#include "Lobby.h"
// #include "Player.cpp"

#include <iterator> 
#include <string>
#include <string.h>
#include <stdio.h>
#include <unordered_map>
#include <algorithm> // find_if

// Server #includes
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <iostream>

#define BACKLOG 10 // Number of connections

int sockfd;

// Read repeeating data passed in by the client and return the number of reads by to the client.
void Lobby::*startGame(void *clientSocket) {

}

int main (int argc, char* argv[])
{
    int port = atoi(argv[1]);

    int sin_size;
    
    // Declaring sockaddr_in structures
    sockaddr_in server;
    bzero((char*) &server, sizeof(server));
    server.sin_family      = AF_INET; // Address Family Internet
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port        = htons(port);

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd != 0)
    {
        printf("Socket creation successfully.\n");
    }
    else
    {
        perror("Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    const int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &on, sizeof(int));

    //Bind the ip address and port to a socket
    if (bind(sockfd, (sockaddr*) &server, sizeof(server)) < 0)
    {
        perror("Socket binding failed.");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket successfully binded.\n");
    }
    
    // Tells socket to listen
    listen(sockfd, BACKLOG);
    if ((listen(sockfd, BACKLOG) != 0))
    {
        perror("Listen failed...");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server listening...\n");
    }

    // A loop that continously listens to new connection by using threads
    while (true)
    {
        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof(newSockAddr);

        int newsockfd = accept(sockfd, (sockaddr* ) &newSockAddr, &newSockAddrSize);

        printf("Server: got connection from %s port %d\n", inet_ntoa(newSockAddr.sin_addr), ntohs(newSockAddr.sin_port));

        if (newsockfd < 0)
        {
            perror("error with newsockfd: ");
            exit(EXIT_FAILURE);
        } 

        pthread_t thread1;
        int re = pthread_create(&thread1, NULL, startGame, (void *) &newsockfd);
    
    }

    close(sockfd);
    return 0;
}