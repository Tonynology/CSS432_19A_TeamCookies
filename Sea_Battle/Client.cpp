#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <string>
#include <iostream>
#include <future>

#include "Etc.h"

#define DEFAULT_CUSERNAME "lllloyd"
#define DEFAULT_SPORT 6932
#define DEFAULT_CPORT 6932
#define DEFAULT_SADDRESS "uw1-320-01"
#define DEFAULT_CADDRESS "uw1-320-00"
#define DEFAULT_CSD -1

std::string cUsername = DEFAULT_CUSERNAME;
int sPort = DEFAULT_SPORT;
int cPort = DEFAULT_CPORT;
std::string sAddress = DEFAULT_SADDRESS;
std::string cAddress = DEFAULT_CADDRESS;
int cSd = DEFAULT_CSD;
sockaddr_in sendSockAddr;
bool registered;
std::string acknowledgement;

void registerUser()
{
    char loginname[LOGIN_NAME_MAX];
    getlogin_r(loginname, LOGIN_NAME_MAX); // user info from computer
    cUsername = std::string(loginname);
    //cUsername = Etc::consoleIn(); // user info from console, prone to errors
    //Etc::consoleOut("cUsername: " + cUsername + "\n");

    srand(time(NULL));
    cPort = 1023 + rand() % (65535 - 1023);
    //cPort = stoi(Etc::consoleIn());
    //Etc::consoleOut("cPort: " + std::to_string(cPort) + "\n");

    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    cAddress = std::string(hostname);
    //cAddress = Etc::consoleIn();
    //Etc::consoleOut("cAddress: " + cAddress + "\n");

    registered = true;
    Etc::consoleOut("\nregistration successful!\n");
    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();
}

void listGames()
{
    std::string games = Etc::portIn(cSd);
	Etc::portOut(cSd, games); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
    Etc::consoleOut("\ngames: \n" + games + "\n");// + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (games == acknowledgement), "Something has gone terribly wrong! " + games + " != " + acknowledgement);

    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();
}

void createGame()
{
	Etc::portOut(cSd, cUsername); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    //Etc::consoleOut("cUsername: " + cUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

	Etc::portOut(cSd, std::to_string(cPort));
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    //Etc::consoleOut("cPort: " + std::to_string(cPort) + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (std::to_string(cPort) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(cPort) + " != " + acknowledgement);

	Etc::portOut(cSd, cAddress);
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    //Etc::consoleOut("cAddress: " + cAddress + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cAddress == acknowledgement), "Something has gone terribly wrong! " + cAddress + " != " + acknowledgement);

    std::string s = "./player.out " + std::to_string(cPort) + " " + cAddress;
    Etc::consoleOut("\n" + s + "\n");
    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();

    //int ac; // call Player::main(s)
    //char **av = Etc::parsedargs(s, &ac);
    //Player::main(ac, av);
    //Etc::freeparsedargs(av);
    system(s.c_str()); // run (s)
}

void joinGame()
{
    listGames();
    Etc::consoleOut("type a username to enter a sea battle with: ");
    std::string pUsername = Etc::consoleIn();

	Etc::portOut(cSd, pUsername); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
    //Etc::consoleOut("pUsername: " + pUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (pUsername == acknowledgement), "Something has gone terribly wrong! " + pUsername + " != " + acknowledgement);
    
    int pPort = stoi(Etc::portIn(cSd));
	Etc::portOut(cSd, std::to_string(pPort));
	acknowledgement = Etc::portIn(cSd);
    //Etc::consoleOut("pPort: \n" + std::to_string(pPort) + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (std::to_string(pPort) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(pPort) + " != " + acknowledgement);

    std::string pAddress = Etc::portIn(cSd);
	Etc::portOut(cSd, pAddress);
	acknowledgement = Etc::portIn(cSd);
    //Etc::consoleOut("pAddress: \n" + pAddress + "\nacknowledgement: \n" + acknowledgement + "\n");
    Etc::errChk(-1 + (pAddress == acknowledgement), "Something has gone terribly wrong! " + pAddress + " != " + acknowledgement);

    std::string s = "./player.out " + std::to_string(cPort) + " " + cAddress + " " + std::to_string(pPort) + " " + pAddress;
    Etc::consoleOut("\n" + s + "\n");
    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();

    //int ac; // call Player::main(s)
    //char **av = Etc::parsedargs(s, &ac);
    //Player::main(ac, av);
    //Etc::freeparsedargs(av);
    system(s.c_str()); // run (s)
}

void unregisterUser()
{
    Etc::consoleOut("cUsername: " + cUsername);
	Etc::portOut(cSd, cUsername); /// three-way handshake
	acknowledgement = Etc::portIn(cSd);
	Etc::portOut(cSd, acknowledgement);
   // Etc::consoleOut("\nacknowledgement: " + acknowledgement + "\n");
    Etc::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

    registered = false;
    Etc::consoleOut("\nunregistration successful!\n");
    Etc::consoleOut("press enter to continue\n");
    Etc::consoleIn();
}

int main(int argc, char const *argv[])
{
    //Etc::consoleOut("[int 0-9] enter the number you've been assigned: "); // hostname input
    //sAddress = "uw1-320-0";
    //sAddress += Etc::consoleIn();

    Etc::clientSetup(sPort, sAddress, cSd, sendSockAddr);
    Etc::portConnect(cSd, sendSockAddr);

    while (true) {
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        Etc::consoleOut("welcome to Sea Battle, a game by Team Cookies\n");
        if (registered) Etc::consoleOut("\nyou are registered as " + cUsername + " on " + cAddress + ":" + std::to_string(cPort) + "\n");
        if (!registered) Etc::consoleOut("[int 1] select from the following options:\n");
        if (registered) Etc::consoleOut("[int 2-6] select from the following options:\n");
	    if (!registered) Etc::consoleOut("[1] register user\n");
	    if (registered) Etc::consoleOut("[2] list games\n");
	    if (registered) Etc::consoleOut("[3] create game\n");
	    if (registered) Etc::consoleOut("[4] join game\n");
	    //if (registered) Etc::consoleOut("[5] exit game\n");
	    if (registered) Etc::consoleOut("[6] unregister user\n");
        if (registered) Etc::consoleOut("\n");

        //Etc::consoleOut("sselection: ");
        std::string sselection = Etc::consoleIn();
        Etc::validateSelection(sselection, registered, cUsername, cAddress, cPort);
        int selection = stoi(sselection);
	    Etc::portOut(cSd, std::to_string(selection)); /// three-way handshake
	    acknowledgement = Etc::portIn(cSd);
	    Etc::portOut(cSd, acknowledgement);
        //Etc::consoleOut("acknowledgement: " + acknowledgement + "\n");
        Etc::errChk(-1 + (std::to_string(selection) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(selection) + " != " + acknowledgement);

        if (selection == 1 && !registered) registerUser();
        else if (selection == 2 && registered) listGames();
        else if (selection == 3 && registered) createGame();
        else if (selection == 4 && registered) joinGame();
        //else if (selection == 5 && registered) exitGame();
        else if (selection == 6 && registered) unregisterUser();
        else {
            Etc::consoleOut("not a valid option...\n");
            Etc::consoleOut("press enter to continue\n");
            Etc::consoleIn();
        }
    }
	close(cSd);
	exit(0);
    return 0;
}
