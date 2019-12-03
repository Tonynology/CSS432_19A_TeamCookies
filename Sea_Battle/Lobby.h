#ifndef LOBBY_H
#define LOBBY_H

#include <vector>
#include <string>
#include <iostream>

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
        bool checkDuplicates();

        struct player {
            std::stirng username;
            int port;
            std::string ipAddress;
        }
};

#endif // LOBBY_H