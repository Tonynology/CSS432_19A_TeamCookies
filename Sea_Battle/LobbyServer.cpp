#include "LobbyServer.h"
#include "Player.cpp"

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

void* LobbyServer::startGame (void* clientSocket) {
    std::cout << "Welcome to Sea Battle v1.4 beta, a game by Team Cookies.\n" << std::endl;

    int clientfd = *((int *) clientSocket);

    LobbyServer ls;
    ls.startMenu(clientfd);
}

void LobbyServer::startMenu(int clientfd) {
    this->clientfd = clientfd;

    int userResponseTemp = 0;
    int userResponse = 0;
    std::string username;

    recv(clientfd, &userResponseTemp, 4, 0);
    userResponse = ntohl(userResponseTemp);
    std::cout << "userResponse: " << userResponse << std::endl;
    
    switch(userResponse) {
        case 1:
            registerUser();
            break;
        // case 2:
        //     listGames();
        //     break;
        case 3:
            createGame();
            break;
        // case 4:
        //     joinGame();
        //     break;
        // case 5:
        //     exitGame();
        //     break;
        // case 6:
        //     unregisterUser();
        //     break;
        default:
            std::cout << "Not a valid option. Please reselect." << std::endl;
            break;
    }
}

void LobbyServer::registerUser() {
    struct PlayerData p;
    int tosend;
    int duplicateBool;

    // Creating username
    memset(&msg, 0, sizeof(msg));
    recv(clientfd, (char*) msg, sizeof(msg), 0);
    std::string username = msg;
    std::cout << "username: " << username << std::endl;

    if (userData.find(username) == userData.end()) {
        duplicateBool = 0;
        tosend = htonl(duplicateBool);
        send(clientfd, (const char*) &tosend, sizeof(duplicateBool), 0);

        // Receive port from client
        int portTemp;
        recv(clientfd, &portTemp, 4, 0);
        this->temp = ntohl(portTemp);
        std::cout << "port: " << this->temp << std::endl;

        // Receive ipAdress from client
        memset(&msg, 0, sizeof(msg));
        recv(clientfd, (char*) msg, sizeof(msg), 0);
        p.ipAddress = msg;
        std::cout << "ipAddress: " << p.ipAddress << std::endl;

        // Add Player struct to database (unorderd_map)
        userData[username] = p;

        std::cout << "map size: " << userData.size() << std::endl;

        std::cout << "port from username: " << p.port << std::endl;

        // Return to startMenu
        startMenu(clientfd);

        std::cout << std::endl;

    } else { // Duplicate username. Asks user to enter different username.
        std::cout << "duplicate username " << std::endl;
        duplicateBool = 1;
        tosend = htonl(duplicateBool);
        send(clientfd, (const char*) &tosend, sizeof(duplicateBool), 0);
        registerUser();
    }
}

// void Lobby::unregisterUser() {
//     struct PlayerData p;
//     std::string username;

//     std::cout << "Please enter the username of the user you want to remove: " << std::endl;
//     std::cin >> username;
// }

// void Lobby::listGames() {
//     std::cout << "running listGames" << std::endl;
// }

void LobbyServer::createGame() {
    struct PlayerData p;
    int tosend, usernameBool;

    std::cout << "map size: " << userData.size() << std::endl;

    memset(&msg, 0, sizeof(msg));
    recv(clientfd, (char*) msg, sizeof(msg), 0);
    std::string username = msg;
    std::cout << "username: " << username << " found" << std::endl;

    userData[username] = p;

    std::cout << "port saved: " << p.port << std::endl;

    if (userData.find(username) != userData.end()) {
        usernameBool = 1;
        tosend = htonl(usernameBool);
        send(clientfd, (const char*) &tosend, sizeof(usernameBool), 0);

        std::cout << "username " << username <<  " found" << std::endl;

        userData[username] = p;
        // std::string s = "./player.out 1111 uw1-320-12";
        std::cout << "p.port: " << this->temp << std::endl;
        std::cout << "p.ipAddress" << p.ipAddress << std::endl;
        std::string s = "./player.out " + std::to_string(p.port) + " " + p.ipAddress;

        std::cout << "command line: " << s << std::endl;

        // int ac;
        // char **av = Etc::parsedargs(s, &ac);
        // Player::main(ac, av);

        // Etc::freeparsedargs(av);
    } else {
        std::cout << "username not found" << std::endl;

        usernameBool = 0;
        tosend = htonl(usernameBool);
        send(clientfd, (const char*) &tosend, sizeof(usernameBool), 0);
    }
}

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

        pthread_t thread1;

        int re = pthread_create(&thread1, NULL, (THREADFUNCPTR) &LobbyServer::startGame, (void*)&newsockfd);
        //int re = pthread_create(&thread1, NULL, (THREADFUNCPTR) &LobbyServer::startGame, lsPtr);
    }

    close(sockfd);
    return 0;
}