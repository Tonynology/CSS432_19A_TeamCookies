#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

#include <limits.h>

#include <string>
#include <iostream>
#include <future>
#include <unordered_map>

#include "Static.h"

#define DEFAULT_SPORT 6932
#define DEFAULT_SERVERSD -1
#define DEFAULT_NEWSD -1

int sPort = DEFAULT_SPORT;
int serverSd = DEFAULT_SERVERSD;
int newSd = DEFAULT_NEWSD;
std::unordered_map<std::string, std::pair<int, std::string>> games; //contains peers whom have selected "join game" and have not recieved a partner.
std::string acknowledgement;

void blackHole(){
    while (true) std::this_thread::sleep_for(std::chrono::hours(1));// stall thread termination for an hour, thereby preventing server crash, lol.
}

void printGames(){
    for(std::unordered_map<std::string, std::pair<int, std::string>>::const_iterator it = games.begin(); it != games.end(); ++it) {
        std::cout << "username: " + it->first << " port: " << it->second.first << " address: " << it->second.second << "\n";
    }
}

void registerUser(int sd)
{
    // client-side function
    printGames();
}

void listGames(int sd)
{
    std::string sgames = "          ";
    for(std::unordered_map<std::string, std::pair<int, std::string>>::const_iterator it = games.begin(); it != games.end(); ++it)
    {
        sgames += it->first + " ";
        //sgames += "username: " + it->first + " port: " + std::to_string(it->second.first) + " address: " + it->second.second + "\n";
    } // TODO: Stream the output to the user, don't concatenate it and make a single write.

	Static::portOut(sd, sgames);
	acknowledgement = Static::portIn(sd);
    Static::portOut(sd, acknowledgement);
    Static::consoleOut("sgames: \n" + sgames + "\nacknowledgement: \n" + acknowledgement + "\n");
    Static::errChk(-1 + (sgames == acknowledgement), "Something has gone terribly wrong!" + sgames + " != " + acknowledgement);

    printGames();
}

void createGame(int sd)
{
	std::string cUsername = Static::portIn(sd); /// three-way handshake
	Static::portOut(sd, cUsername);
	acknowledgement = Static::portIn(sd);
    Static::consoleOut("cUsername: " + cUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

    int cPort = std::stoi(Static::portIn(sd));
	Static::portOut(sd, std::to_string(cPort));
	acknowledgement = Static::portIn(sd);
    Static::consoleOut("cPort: " + std::to_string(cPort) + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (std::to_string(cPort) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(cPort) + " != " + acknowledgement);

    Static::consoleOut("cAddress: ");
    std::string cAddress = Static::portIn(sd);
	Static::portOut(sd, cAddress);
	acknowledgement = Static::portIn(sd);
    Static::consoleOut(cAddress + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (cAddress == acknowledgement), "Something has gone terribly wrong! " + cAddress + " != " + acknowledgement);

    //games.insert(make_pair(cUsername, make_pair(cPort, cAddress))); // does not overwrite duplicates
    games[cUsername] = make_pair(cPort,cAddress); // overwrites duplicates
    printGames();
}

void joinGame(int sd)
{
    listGames(sd);

	std::string pUsername = Static::portIn(sd); /// three-way handshake
	Static::portOut(sd, pUsername);
	acknowledgement = Static::portIn(sd);
    Static::consoleOut("pUsername: " + pUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (pUsername == acknowledgement), "Something has gone terribly wrong! " + pUsername + " != " + acknowledgement);

    Static::portOut(sd, std::to_string(games[pUsername].first));
	acknowledgement = Static::portIn(sd);
    Static::portOut(sd, acknowledgement);
    Static::consoleOut("pPort: \n" + std::to_string(games[pUsername].first) + "\nacknowledgement: \n" + acknowledgement + "\n");
    Static::errChk(-1 + (std::to_string(games[pUsername].first) == acknowledgement), "Something has gone terribly wrong!" + std::to_string(games[pUsername].first) + " != " + acknowledgement);

	Static::portOut(sd, games[pUsername].second);
	acknowledgement = Static::portIn(sd);
    Static::portOut(sd, acknowledgement);
    Static::consoleOut("pAddress: \n" + games[pUsername].second + "\nacknowledgement: \n" + acknowledgement + "\n");
    Static::errChk(-1 + (games[pUsername].second == acknowledgement), "Something has gone terribly wrong!" + games[pUsername].second + " != " + acknowledgement);

    games.erase(pUsername);
    printGames();
}

void exitGame(int sd){
    Static::consoleOut("exit signal recieved from: " + sd);
}

void unregisterUser(int sd)
{
	std::string cUsername = Static::portIn(sd); /// three-way handshake
	Static::portOut(sd, cUsername);
	acknowledgement = Static::portIn(sd);
    Static::consoleOut("cUsername: " + cUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

    games.erase(cUsername);
    printGames();
}

void *server(void *)
{
    int sd = newSd;
    Static::consoleOut("sd: " + sd);
    while (true) {
        try{
        std::string sselection = Static::portIn(sd); /// three-way handshake
        int selection = stoi(sselection); // safety is guaranteed through integrity of client validation, not server validation
	    Static::portOut(sd, std::to_string(selection));
	    acknowledgement = Static::portIn(sd);
        Static::consoleOut("selection: " + std::to_string(selection) + "\nacknowledgement: " + acknowledgement + "\n");
        Static::errChk(-1 + (std::to_string(selection) == acknowledgement), "Something has gone terribly wrong!" + std::to_string(selection) + " != " + acknowledgement);

        if (selection == 1) registerUser(sd);
        else if (selection == 2) listGames(sd);
        else if (selection == 3) createGame(sd);
        else if (selection == 4) joinGame(sd);
        else if (selection == 5) blackHole();
        else if (selection == 6) unregisterUser(sd);
        else {
            Static::consoleOut("not a valid option...\n");
        }
        }
        catch(const std::invalid_argument& ia){
            std::cerr << "Panic!" << ia.what();
            blackHole();
        }
    }

	close(serverSd);
    close(sd);
	exit(0);
    return 0;
}

int main(int argc, char const *argv[])
{
    Static::serverSetup(sPort, serverSd);
    Static::consoleOut("Welcome to Sea Battle, a game by Team Cookies\n");
    while (true) {
        sockaddr_in newSockAddr;
        newSd = Static::portAccept(serverSd, newSockAddr);
        
	    pthread_t t;
        //pthread_attr_t attr;
        //pthread_attr_init(&attr);
        
	    Static::errChk(pthread_create(&t, NULL, server, NULL ), "Error: Thread failed to create.");
	    Static::errChk(pthread_detach(t), "Error: Thread failed to detach.");
        //Static::errChk(pthread_join(t, NULL), "Error: Thread failed to join.");
        //pthread_cancel(t);
        //pthread_exit(NULL)
    }
    exit(0);
    return 0;
}
