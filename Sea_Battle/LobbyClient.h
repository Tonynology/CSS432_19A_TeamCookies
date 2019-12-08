#ifndef LOBBYCLIENT_H
#define LOBBYCLIENT_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

class LobbyClient {
    public:
        void startGame();
        void startMenu();

        void registerUser();
        void listGames();

    private:

};

#endif // LOBBYCLIENT_H