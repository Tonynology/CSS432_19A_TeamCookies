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
    Static::consoleOut("sgames: \n" + sgames + "\n");

    printGames();
}

void createGame(int sd)
{
    std::string sgames = "          ";
    for(std::unordered_map<std::string, std::pair<int, std::string>>::const_iterator it = games.begin(); it != games.end(); ++it) {
        sgames += it->first + " ";
        //sgames += "username: " + it->first + " port: " + std::to_string(it->second.first) + " address: " + it->second.second + "\n";
    }
	Static::portOut(sd, sgames);
    Static::consoleOut("sgames: \n" + sgames + "\n");

	std::string cUsername = Static::portIn(sd); /// three-way handshake
    Static::consoleOut("cUsername: " + cUsername + "\n");

    int cPort = std::stoi(Static::portIn(sd));
    Static::consoleOut("cPort: " + std::to_string(cPort) + "\n");

    std::string cAddress = Static::portIn(sd);
    Static::consoleOut("cAddress: " + cAddress + "\n");

    //games.insert(make_pair(cUsername, make_pair(cPort, cAddress))); // does not overwrite duplicates
    games[cUsername] = make_pair(cPort,cAddress); // overwrites duplicates
    printGames();
}

void joinGame(int sd)
{
    listGames(sd);

	std::string pUsername = Static::portIn(sd); /// three-way handshake
    Static::consoleOut("pUsername: " + pUsername + "\n");

    Static::portOut(sd, std::to_string(games[pUsername].first));
    Static::consoleOut("pPort: \n" + std::to_string(games[pUsername].first) + "\n");

	Static::portOut(sd, games[pUsername].second);
    Static::consoleOut("pAddress: \n" + games[pUsername].second + "\n");

    games.erase(pUsername);
    printGames();
}

void exitGame(int sd){
    Static::consoleOut("exit signal recieved from: " + sd);
}

void unregisterUser(int sd)
{
	std::string cUsername = Static::portIn(sd);
    Static::consoleOut("cUsername: " + cUsername + "\n");

    games.erase(cUsername);
    printGames();
}

void *server(void *)
{
    int sd = newSd;
    Static::consoleOut("sd: " + sd);
    while (true) {
        try{
            std::string sselection = Static::portIn(sd);
            int selection = stoi(sselection); // safety is guaranteed through integrity of client validation, not server validation
            Static::consoleOut("selection: " + std::to_string(selection) + "\n");

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
            std::cerr << "panic!" << ia.what();
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
    if (argc == 1){ // prompt for port
    //    Static::consoleOut("enter desired server port: "); //default port will work fine 99.99% of the time
    //    sPort = Static::consoleIn();
    }
    else if (argc == 2){ // argument for port
        sPort = std::stoi(argv[1]);
    }
    else{
		Static::errChk(-1, "usage: ./server.out [server port]");
    }

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
