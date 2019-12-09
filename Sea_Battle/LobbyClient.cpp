#include "LobbyClient.h"

#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstring>

void LobbyClient::startGame(int sockfd) {
    this->socket = sockfd;
    std::cout << "Welcome to Sea Battle v1.4 beta, a game by Team Cookies.\n" << std::endl;
    startMenu();
}

void LobbyClient::startMenu() {
    std::cout << "Please select the following options by typing in the number:" << std::endl;
	std::cout << "[1] Register" << std::endl;
	std::cout << "[2] List Games" << std::endl;
	std::cout << "[3] Create Game" << std::endl;
	std::cout << "[4] Join Game" << std::endl;
	std::cout << "[5] Exit Game" << std::endl;
	std::cout << "[6] Unregister" << std::endl;

    std::cout << std::endl;

    int userResponse = 0;
    std::cin >> userResponse;

    // Send userResponse to server
    int tosend = htonl(userResponse);
    send(socket, (const char*) &tosend, sizeof(userResponse), 0);
    std::cout << "send response to server" << std::endl;

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
         case 6:
             unregisterUser();
             break;
        default:
            std::cout << "Not a valid option. Please reselect." << std::endl;
            break;
    }
}

void LobbyClient::registerUser() {

    std::string username;
    std::cout << "Please enter an username for this user: " << std::endl;
    std::cin >> username;

    // Send username to server
    memset(&msg, 0, sizeof(msg));
    strcpy(msg, username.c_str());
    send(socket, (char *) msg, strlen(msg), 0);

    // Server responds whether username has duplicates
    int temp, duplicateBool;
    recv(socket, &temp, 90, 0);
    duplicateBool = ntohl(temp);

    // std::cout << "is there duplicates: " << duplicateBool << std::endl;

    if (duplicateBool == 0) {
        // User enters port number
        int port = 0;
        std::cout << "Please enter the last 4 digits of your ID number as your port number unless it is < 1024, in which case, add 1024 to your ID number." << std::endl;
        std::cin >> port;

        // Client sends port to server
        int tosend = htonl(port);
        send(socket, (const char*) &tosend, sizeof(port), 0);
        std::cout << "send port to server" << std::endl;

        // User enters ipAddress
        std::string ipAddress;
        std::cout << "Please enter an UWB network address. They are named uw1-320-01 to uw1-320-15. Example of an ipAddress would be 'uw1-320-08'." << std::endl;
        std::cin >> ipAddress;

        // Client sends ipAddress to server
        memset(&msg, 0, sizeof(msg));
        strcpy(msg, ipAddress.c_str());
        send(socket, (char * ) msg, strlen(msg), 0);
        std::cout << "ipAddress send to server" << std::endl;

        std::cout << "User created. Returning to start menu." << std::endl;
        std::cout << std::endl;

        startMenu();
    } else {
        std::cout << std::endl;
        std::cout << "Username already taken. Please enter a different username." << std::endl;
        registerUser();
    }
}

void LobbyClient::createGame() {
    std::string username;
    std::cout << "Please enter the username you want to join the game with." << std::endl;
    std::cin >> username;

    // Send username to server
    memset(&msg, 0, sizeof(msg));
    strcpy(msg, username.c_str());
    send(socket, (char *) msg, strlen(msg), 0);

    int temp, usernameBool;
    recv(socket, &temp, 90, 0);
    usernameBool = ntohl(temp);

    std::cout << "usernameBool: " << usernameBool << std::endl;

    if (usernameBool == 1) {
        
    } else {
        std::cout << "Username not found. Returning to start menu." << std::endl;
        std::cout << std::endl;

        startMenu();
    }
}

void LobbyClient::unregisterUser() {
	std::string username;
	std::cout << "Please enter the username you want to remove: " << std::endl;
	std::cin >> username;

	// Send username to server
	memset(&msg, 0, sizeof(msg));
	strcpy(msg, username.c_str());
	send(socket, (char*)msg, strlen(msg), 0);

	// Server responds whether username has duplicates
	int temp, usernameBool;
	recv(socket, &temp, 90, 0);
	usernameBool = ntohl(temp);

	std::cout << "usernameBool: " << usernameBool << std::endl;

	if (usernameBool == 1) {
		std::cout << "username " << username << " found" << std::endl;
		std::cout << "Remove the user name:" << username << std::endl;
		startMenu();
	}
	else {
		std::cout << "Username not found. Returning to start menu." << std::endl;
		std::cout << std::endl;

		startMenu();
	}
}

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

    LobbyClient lc;
    lc.startGame(sockfd);

    // char* hello = "hello from client";
    // send(sockfd, "Hello, world!\n", 13, 0);

    //std::cout << "msg sent to server" << std::endl;

    // std::cout << "client port: " << port << std::endl;
    // int portChar = htonl(port);

    // char* tosend = (char*) &port;

    // send(sockfd, tosend, sizeof(port), 0);
    // printf("port send to lobby-server\n");

    close(sockfd);
    return 0;
}