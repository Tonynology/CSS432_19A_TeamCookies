#include "LobbyServer.h"
// #include "Player.cpp"

#include <iostream>
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
#include <pthread.h>

#define BACKLOG 10 // Number of connections

int sockfd;

void *LobbyServer::startGame (void *) {
    std::cout << "Welcome to Sea Battle v1.4 beta, a game by Team Cookies.\n" << std::endl;
    //startMenu();
}

// void Lobby::startMenu() {
//     std::cout << "Please select the following options by typing in the number:" << std::endl;
// 	std::cout << "[1] Register" << std::endl;
// 	std::cout << "[2] List Games" << std::endl;
// 	std::cout << "[3] Create Game" << std::endl;
// 	std::cout << "[4] Join Game" << std::endl;
// 	std::cout << "[5] Exit Game" << std::endl;
// 	std::cout << "[6] Unregister" << std::endl;

//     std::cout << std::endl;

//     int userResponse = 0;
//     std::cin >> userResponse;

//     switch(userResponse) {
//         case 1:
//             registerUser();
//             break;
//         case 2:
//             listGames();
//             break;
//         case 3:
//             createGame();
//             break;
//         case 4:
//             joinGame();
//             break;
//         case 5:
//             exitGame();
//             break;
//         case 6:
//             unregisterUser();
//             break;
//         default:
//             std::cout << "Not a valid option. Please reselect." << std::endl;
//             break;
//     }
// }

// void LobbyServer::registerUser() {
//     struct PlayerData p;
//     std::string username;

//     std::cout << "Please enter an username for this user: " << std::endl;
//     std::cin >> username;

//     if (userData.find(username) == userData.end()) {
//         userData[username] = p;
//         std::cout << "Please enter a port number between 2000 - 5000" << std::endl;
//         std::cin >> p.port;

//         std::cout << "Please enter your IP address" << std::endl;
//         std::cin >> p.ipAddress;

//         std::cout << "User " << username << " registered." << std::endl;
//         std::cout << "Returning to start menu" << std::endl;
//         std::cout << std::endl;
//         startMenu();
//     } else {
//         std::cout << "Username already taken. Please choose another one." << std::endl;
//         std::cout << std::endl;
//         registerUser();
//     }
// }

// void Lobby::unregisterUser() {
//     struct PlayerData p;
//     std::string username;

//     std::cout << "Please enter the username of the user you want to remove: " << std::endl;
//     std::cin >> username;
// }

// void Lobby::listGames() {
//     std::cout << "running listGames" << std::endl;
// }

// void Lobby::createGame() {
//     std::cout << "running createGame" << std::endl;
// }

// void Lobby::joinGame() {
//     std::cout << "running joinGame" << std::endl;
// }

// void Lobby::exitGame() {
//     std::cout << "running exitGame" << std::endl;
// }

typedef void * (*THREADFUNCPTR)(void *);

int main (int argc, char* argv[]) {
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

        int portTemp;
        int portNum;

        recv(newsockfd, &portTemp, sizeof(portNum), 0);
        portNum = ntohl(portTemp);

        std::cout << "portNum: " << portTemp << std::endl;

        pthread_t thread1;
        int re = pthread_create(&thread1, NULL, (THREADFUNCPTR) &LobbyServer::startGame, (void*) &newsockfd);
    
    }

    close(sockfd);
    return 0;
}