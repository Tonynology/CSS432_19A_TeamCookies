#ifndef LOBBY_H
#define LOBBY_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

class Lobby {
    public:
        void startGame();
        void startMenu();
        void registerUser();
        void listGames();
        void createGame();
        void joinGame();
        void exitGame();
        void unregisterUser();

    private:
        struct Player {
            std::string username;
            int port;
            std::string ipAddress;
        };

        // Player p;
        std::vector<Player> playerVector;

        std::unordered_map<std::string, std::vector<Player>> userData;

        // std::unordered_map<std::string, Player> userData;

        void printMap();
};

#endif // LOBBY_H