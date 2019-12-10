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

#include "Etc.h"

#define DEFAULT_SPORT 6932
#define DEFAULT_SERVERSD -1
#define DEFAULT_NEWSD -1

int sPort = DEFAULT_SPORT;
int serverSd = DEFAULT_SERVERSD;
int newSd = DEFAULT_NEWSD;
std::unordered_map<std::string, std::pair<int, std::string>> database; //contains players whom have selected "join game" and have not recieved a partner.
std::string acknowledgement;

void printDatabase(){
    for(std::unordered_map<std::string, std::pair<int, std::string>>::const_iterator it = database.begin(); it != database.end(); ++it) {
        std::cout << it->first << " " << it->second.first << " " << it->second.second << "\n";
    }
}

void registerUser(int sd)
{
    // client-side function
    printDatabase();
}

void listGames(int sd)
{
    std::string games;
    for(std::unordered_map<std::string, std::pair<int, std::string>>::const_iterator it = database.begin(); it != database.end(); ++it)
    {
        games += it->first + " ";// + std::to_string(it->second.first) + " " + it->second.second + "\n";
    } // TODO: Stream the output to the user, don't concatenate it and make a single write.

	Etc::portOut(sd, games);
	acknowledgement = Etc::portIn(sd);
    Etc::portOut(sd, acknowledgement);
    Etc::consoleOut("games: \n" + games + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (games == acknowledgement), "Something has gone terribly wrong!" + games + " != " + acknowledgement);

    printDatabase();
}

void createGame(int sd)
{
	std::string cUsername = Etc::portIn(sd); /// three-way handshake
	Etc::portOut(sd, cUsername);
	acknowledgement = Etc::portIn(sd);
    Etc::consoleOut("cUsername: " + cUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

    int cPort = std::stoi(Etc::portIn(sd));
	Etc::portOut(sd, std::to_string(cPort));
	acknowledgement = Etc::portIn(sd);
    Etc::consoleOut("cPort: " + std::to_string(cPort) + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (std::to_string(cPort) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(cPort) + " != " + acknowledgement);

    Etc::consoleOut("cAddress: ");
    std::string cAddress = Etc::portIn(sd);
	Etc::portOut(sd, cAddress);
	acknowledgement = Etc::portIn(sd);
    Etc::consoleOut(cAddress + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cAddress == acknowledgement), "Something has gone terribly wrong! " + cAddress + " != " + acknowledgement);

    //database.insert(make_pair(cUsername, make_pair(cPort, cAddress))); // does not overwrite duplicates
    database[cUsername] = make_pair(cPort,cAddress); // overwrites duplicates
    printDatabase();
}

void joinGame(int sd)
{
    listGames(sd);

	std::string pUsername = Etc::portIn(sd); /// three-way handshake
	Etc::portOut(sd, pUsername);
	acknowledgement = Etc::portIn(sd);
    Etc::consoleOut("pUsername: " + pUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (pUsername == acknowledgement), "Something has gone terribly wrong! " + pUsername + " != " + acknowledgement);

    Etc::portOut(sd, std::to_string(database[pUsername].first));
	acknowledgement = Etc::portIn(sd);
    Etc::portOut(sd, acknowledgement);
    Etc::consoleOut("pPort: \n" + std::to_string(database[pUsername].first) + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (std::to_string(database[pUsername].first) == acknowledgement), "Something has gone terribly wrong!" + std::to_string(database[pUsername].first) + " != " + acknowledgement);

	Etc::portOut(sd, database[pUsername].second);
	acknowledgement = Etc::portIn(sd);
    Etc::portOut(sd, acknowledgement);
    Etc::consoleOut("pAddress: \n" + database[pUsername].second + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (database[pUsername].second == acknowledgement), "Something has gone terribly wrong!" + database[pUsername].second + " != " + acknowledgement);

    database.erase(pUsername);
    printDatabase();
}

void unregisterUser(int sd)
{
	std::string cUsername = Etc::portIn(sd); /// three-way handshake
	Etc::portOut(sd, cUsername);
	acknowledgement = Etc::portIn(sd);
    Etc::consoleOut("cUsername: " + cUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

    database.erase(cUsername);
    printDatabase();
}

void *server(void *)
{
    int sd = newSd;
    Etc::consoleOut("sd: " + sd);
    while (true) {
        std::string sselection = Etc::portIn(sd); /// three-way handshake
        int selection = stoi(sselection); // safety is guaranteed through handshake, not through validation
	    Etc::portOut(sd, std::to_string(selection));
	    acknowledgement = Etc::portIn(sd);
        Etc::consoleOut("selection: " + std::to_string(selection) + "\nacknowledgement: " + acknowledgement + "\n");
        Etc::errChk(-1 + (std::to_string(selection) == acknowledgement), "Something has gone terribly wrong!" + std::to_string(selection) + " != " + acknowledgement);

        if (selection == 1) registerUser(sd);
        else if (selection == 2) listGames(sd);
        else if (selection == 3) createGame(sd);
        else if (selection == 4) joinGame(sd);
        //else if (selection == 5) exitGame(sd);
        else if (selection == 6) unregisterUser(sd);
        else {
            Etc::consoleOut("not a valid option...\n");
        }
    }
	close(serverSd);
    close(sd);
	exit(0);
    return 0;

}

int main(int argc, char const *argv[])
{
    Etc::serverSetup(sPort, serverSd);
    Etc::consoleOut("Welcome to Sea Battle, a game by Team Cookies\n");
    while (true) {
        sockaddr_in newSockAddr;
        newSd = Etc::portAccept(serverSd, newSockAddr);
        
	    pthread_t t;
        //pthread_attr_t attr;
        //pthread_attr_init(&attr);
        
	    Etc::errChk(pthread_create(&t, NULL, server, NULL ), "Error: Thread failed to create.");
	    Etc::errChk(pthread_detach(t), "Error: Thread failed to detach.");
        //Etc::errChk(pthread_join(t, NULL), "Error: Thread failed to join.");
        //pthread_cancel(t);
        //pthread_exit(NULL)
    }
    exit(0);
    return 0;
}
