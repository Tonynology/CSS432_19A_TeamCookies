#include "Lobby.h"
// #include "Server.cpp"

#include <iostream>
#include <string>

void Lobby::startGame() {
    std::cout << "Welcome to Sea Battle v1.4 beta, a game by Team Cookies\n. Please select the following options." << std::endl;
    std::cout << "Please select the following options by typing in the number:" << std::endl;
	std::cout << "[1] Register" << std::endl;
	std::cout << "[2] List Games" << std::endl;
	std::cout << "[3] Create Game" << std::endl;
	std::cout << "[4] Join Game" << std::endl;
	std::cout << "[5] Exit Game" << std::endl;
	std::cout << "[6] Unregister" << std::endl;

    int userResponse = 0;
    std::cin >> userResponse;

    switch(userResponse) {
        case 1:
            registerUser();
            break;
        case 2:
            listGames();
            break;
        case 3:
            createGame();
            break;
        case 4:
            joinGame();
            break;
        case 5:
            exitGame();
            break;
        case 6:
            unregisterUser();
            break;
        default:
            std::cout << "Not a valid option. Please reselect." << std::endl;
            break;
    }
}

void Lobby::registerUser() {
    std::cout << "running registerUser" << std::endl;

    std::cout << "Please enter a name for this user: " << std::endl;
    std::string username = "";
    cin >> username;

    // set up arraylist to store user information
    
}

void Lobby::listGames() {
    std::cout << "running listGames" << std::endl;
}

void Lobby::createGame() {
    std::cout << "running createGame" << std::endl;
}

void Lobby::joinGame() {
    std::cout << "running joinGame" << std::endl;
}

void Lobby::exitGame() {
    std::cout << "running exitGame" << std::endl;
}

void Lobby::unregisterUser() {
    std::cout << "running unregisterUser" << std::endl;
}