#ifndef LOBBYCLIENT_H
#define LOBBYCLIENT_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

class LobbyClient {
    public:
        void startGame(int sockfd);
        void startMenu();

        void registerUser();
        void createGame();
        void listGames();

        int socket;

    private:
        char msg[1500];

};

#endif // LOBBYCLIENT_H