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

#include "Static.h"

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
    //cUsername = Static::consoleIn(); // user info from console, prone to errors
    //Static::consoleOut("cUsername: " + cUsername + "\n");

    srand(time(NULL));
    cPort = 1023 + rand() % (65535 - 1023);
    //cPort = stoi(Static::consoleIn());
    //Static::consoleOut("cPort: " + std::to_string(cPort) + "\n");

    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    cAddress = std::string(hostname);
    //cAddress = Static::consoleIn();
    //Static::consoleOut("cAddress: " + cAddress + "\n");

    registered = true;
    Static::consoleOut("\nregistration successful!\n");
    Static::consoleOut("press enter to continue\n");
    Static::consoleIn();
}

void listGames()
{
    std::string games = Static::portIn(cSd);
	Static::portOut(cSd, games); /// three-way handshake
	acknowledgement = Static::portIn(cSd);
    Static::consoleOut("\ngames: \n" + games + "\n");// + "\nacknowledgement: \n" + acknowledgement + "\n");
    Static::errChk(-1 + (games == acknowledgement), "Something has gone terribly wrong! " + games + " != " + acknowledgement);

    //Static::consoleOut("press enter to continue\n");
    //Static::consoleIn();
}

void createGame()
{
	Static::portOut(cSd, cUsername); /// three-way handshake
	acknowledgement = Static::portIn(cSd);
	Static::portOut(cSd, acknowledgement);
    //Static::consoleOut("cUsername: " + cUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

	Static::portOut(cSd, std::to_string(cPort));
	acknowledgement = Static::portIn(cSd);
	Static::portOut(cSd, acknowledgement);
    //Static::consoleOut("cPort: " + std::to_string(cPort) + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (std::to_string(cPort) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(cPort) + " != " + acknowledgement);

	Static::portOut(cSd, cAddress);
	acknowledgement = Static::portIn(cSd);
	Static::portOut(cSd, acknowledgement);
    //Static::consoleOut("cAddress: " + cAddress + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (cAddress == acknowledgement), "Something has gone terribly wrong! " + cAddress + " != " + acknowledgement);

    std::string s = "./peer.out " + std::to_string(cPort) + " " + cAddress;
    Static::consoleOut("\nrunning: " + s + "\n");
    //Static::consoleOut("press enter to continue\n");
    //Static::consoleIn();

    //int ac; // call Player::main(s)
    //char **av = Static::parsedargs(s, &ac);
    //Player::main(ac, av);
    //Static::freeparsedargs(av);
    system(s.c_str()); // run (s)
}

void joinGame()
{
    listGames();
    Static::consoleOut("type a username to enter a sea battle with: ");
    std::string pUsername = Static::consoleIn();

	Static::portOut(cSd, pUsername); /// three-way handshake
	acknowledgement = Static::portIn(cSd);
	Static::portOut(cSd, acknowledgement);
    //Static::consoleOut("pUsername: " + pUsername + "\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (pUsername == acknowledgement), "Something has gone terribly wrong! " + pUsername + " != " + acknowledgement);
    
    int pPort = stoi(Static::portIn(cSd));
	Static::portOut(cSd, std::to_string(pPort));
	acknowledgement = Static::portIn(cSd);
    //Static::consoleOut("pPort: \n" + std::to_string(pPort) + "\nacknowledgement: \n" + acknowledgement + "\n");
    Static::errChk(-1 + (std::to_string(pPort) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(pPort) + " != " + acknowledgement);

    std::string pAddress = Static::portIn(cSd);
	Static::portOut(cSd, pAddress);
	acknowledgement = Static::portIn(cSd);
    //Static::consoleOut("pAddress: \n" + pAddress + "\nacknowledgement: \n" + acknowledgement + "\n");
    Static::errChk(-1 + (pAddress == acknowledgement), "Something has gone terribly wrong! " + pAddress + " != " + acknowledgement);

    std::string s = "./peer.out " + std::to_string(cPort) + " " + cAddress + " " + std::to_string(pPort) + " " + pAddress;
    Static::consoleOut("\nrunning: " + s + "\n");
    //Static::consoleOut("press enter to continue\n");
    //Static::consoleIn();

    //int ac; // call Player::main(s)
    //char **av = Static::parsedargs(s, &ac);
    //Player::main(ac, av);
    //Static::freeparsedargs(av);
    system(s.c_str()); // run (s)
}

void exitGame(){
}

void unregisterUser()
{
    Static::consoleOut("cUsername: " + cUsername);
	Static::portOut(cSd, cUsername); /// three-way handshake
	acknowledgement = Static::portIn(cSd);
	Static::portOut(cSd, acknowledgement);
   // Static::consoleOut("\nacknowledgement: " + acknowledgement + "\n");
    Static::errChk(-1 + (cUsername == acknowledgement), "Something has gone terribly wrong! " + cUsername + " != " + acknowledgement);

    registered = false;
    Static::consoleOut("\nunregistration successful!\n");
    Static::consoleOut("press enter to continue\n");
    Static::consoleIn();
}

int main(int argc, char const *argv[])
{
    if (argc == 2) sAddress = argv[1]; // accepts argument for hostname input
    //Static::consoleOut("[int 0-9] enter the number you've been assigned: "); // prompts for hostname input
    //sAddress = "uw1-320-0";
    //sAddress += Static::consoleIn();

    Static::clientSetup(sPort, sAddress, cSd, sendSockAddr);
    Static::portConnect(cSd, sendSockAddr);

    while (true) {
        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        Static::consoleOut("welcome to Sea Battle, a game by Team Cookies\n");
        if (registered) Static::consoleOut("\nyou are registered as " + cUsername + " on " + cAddress + ":" + std::to_string(cPort) + "\n");
        if (!registered) Static::consoleOut("[int 1] select from the following options:\n");
        if (registered) Static::consoleOut("[int 2-6] select from the following options:\n");
	    if (!registered) Static::consoleOut("[1] register user\n");
	    if (registered) Static::consoleOut("[2] list games\n");
	    if (registered) Static::consoleOut("[3] create game\n");
	    if (registered) Static::consoleOut("[4] join game\n");
	    if (registered) Static::consoleOut("[5] exit game\n");
	    if (registered) Static::consoleOut("[6] unregister user\n");
        if (registered) Static::consoleOut("\n");

        //Static::consoleOut("sselection: ");
        std::string sselection = Static::consoleIn();
        //Static::consoleOut("sselection: " + sselection); // Forces the client to flush rogue "^C" input before sending to server. But somehow, that input isn't what is crashing the server...? I think it has to do with some error from the client propogating up to the server through a thread?
        Static::validateSelection(sselection, registered, cUsername, cAddress, cPort);
        int selection = stoi(sselection);
	    Static::portOut(cSd, std::to_string(selection)); /// three-way handshake
	    acknowledgement = Static::portIn(cSd);
	    Static::portOut(cSd, acknowledgement);
        //Static::consoleOut("acknowledgement: " + acknowledgement + "\n");
        Static::errChk(-1 + (std::to_string(selection) == acknowledgement), "Something has gone terribly wrong! " + std::to_string(selection) + " != " + acknowledgement);

        if (selection == 1 && !registered) registerUser();
        else if (selection == 2 && registered){
            listGames();
            Static::consoleOut("press enter to continue\n");
            Static::consoleIn();
        }
        else if (selection == 3 && registered) createGame();
        else if (selection == 4 && registered) joinGame();
        else if (selection == 5 && registered){
            Static::consoleOut("thanks for playing Sea Battle, a game by Team Cookies");
            return 0; //exitGame();
        }
        else if (selection == 6 && registered) unregisterUser();
        else {
            Static::consoleOut("not a valid option...\n");
            Static::consoleOut("press enter to continue\n");
            Static::consoleIn();
        }
    }
	close(cSd);
	exit(0);
    return 0;
}
