#ifndef LOBBYSERVER_H
#define LOBBYSERVER_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

class LobbyServer {
    public:

        // friend std::ostream& operator <<(std::ostream &os, const std::unordered_map <std::string, Player> &m);

        static void* (startGame)(void* clientSocket);
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

#endif // LOBBYSERVER_H