#ifndef LOBBYSERVER_H
#define LOBBYSERVER_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

class LobbyServer {
    public:

        // friend std::ostream& operator <<(std::ostream &os, const std::unordered_map <std::string, Player> &m);

        static void* startGame(void* clientSocket);
        void startMenu(int clientfd);
        void registerUser();
        void listGames();
        void createGame();
        void joinGame();
        void exitGame();
        void unregisterUser();

    private:
        struct PlayerData {
            int port;
            std::string ipAddress;
        };

        int temp;

        int username;
        int clientfd;
        char msg[1500];

        std::unordered_map<std::string, PlayerData> userData;
};

#endif // LOBBYSERVER_H