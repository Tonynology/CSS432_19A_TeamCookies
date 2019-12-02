#ifndef LOBBY_H
#define LOBBY_H

#include <vector>
#include <string>
#include <iostream>

class Lobby {
    public:
        // Vector << operator overload
        //std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& v);

        // Lobby();
        void startGame();
        void startMenu();
        void registerUser();
        void listGames();
        void createGame();
        void joinGame();
        void exitGame();
        void unregisterUser();

    private:
        std::vector<std::string> listOfUsers;

        bool checkDuplicates();
};

#endif // LOBBY_H