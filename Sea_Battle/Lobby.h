#ifndef LOBBY_H
#define LOBBY_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

class Lobby {
    public:

        // friend std::ostream& operator <<(std::ostream &os, const std::unordered_map <std::string, Player> &m);

        void* startGame(void* clientSocket);
        void startMenu();
        void registerUser();
        void listGames();
        void createGame();
        void joinGame();
        void exitGame();
        void unregisterUser();

    private:
        struct PlayerData {
            // std::string username;
            int port;
            std::string ipAddress;
        };

        int sockfd;

        std::unordered_map<std::string, PlayerData> userData;
};

#endif // LOBBY_H