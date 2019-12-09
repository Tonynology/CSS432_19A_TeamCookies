#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

#include <string>
#include <iostream>
#include <future>

#include "Player.h"
#include "Board.h"
#include "Etc.h"

#define DEFAULT_IPORT 6932
#define DEFAULT_IADDRESS "localhost"
#define DEFAULT_UPORT 6933 // port and address cannot both be the same
#define DEFAULT_UADDRESS "localhost"
#define DEFAULT_SERVERSD -1
#define DEFAULT_NEWSD -1
#define DEFAULT_CLIENTSD -1

int iPort = DEFAULT_IPORT;
std::string iAddress = DEFAULT_IADDRESS;
int uPort = DEFAULT_UPORT;
std::string uAddress = DEFAULT_UADDRESS;
int serverSd = DEFAULT_SERVERSD;
int newSd = DEFAULT_NEWSD;
int clientSd = DEFAULT_CLIENTSD;

int Player::getIPort( ) { return iPort; }
std::string Player::getIAddress( ) { return iAddress; }
int Player::getUPort( ) { return uPort; }
std::string Player::getUAddress( ) { return uAddress; }
int Player::getServerSd( ) { return serverSd; }
int Player::getNewSd( ) { return newSd; }
int Player::getClientSd( ) { return clientSd; }

void Player::setIPort( int i ) { iPort = i; }
void Player::setIAddress( std::string i ) { iAddress = i; }
void Player::setUPort( int u ) { uPort = u; }
void Player::setUAddress( std::string u ) { uAddress = u; }
void Player::setServerSd( int s ) { serverSd = s; }
void Player::setNewSd( int n ) { newSd = n; }
void Player::setClientSd( int c ) { clientSd = c; }

int Player::main( int argc, char *argv[] ) {
	Etc::consoleOut("\nPreparing local resources...");
	
	if (argc == 0){
		Etc::errChk(-1, "Usage: ./player.out [iport] [uport] [uaddress]");
    }
	else if (argc == 3){ // two args: first player waits for a challenger
		Player::setIPort(std::stoi(argv[1]));
		Player::setIAddress(argv[2]);
	}
	else if (argc == 4){ // three args: players know each other
		Player::setIPort(std::stoi(argv[1]));
		Player::setUPort(std::stoi(argv[2]));
		Player::setUAddress(argv[3]);
	}
    else if (argc == 5){ // four args: second player sends its connection info to challenger
		Player::setIPort(std::stoi(argv[1]));
		Player::setIAddress(argv[2]);
		Player::setUPort(std::stoi(argv[3]));
		Player::setUAddress(argv[4]);
	}
	else {
		Etc::errChk(-1, "Usage: ./player.out [iport] [uport] [uaddress]");
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	Etc::consoleOut("\nLocal resources prepared!");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	Etc::consoleOut("\nEstablishing remote connection...");
	
	sockaddr_in newSockAddr;
    Etc::serverSetup(Player::getIPort(), serverSd);
	if (argc == 3) {
        pthread_t t;
        Etc::startDots(t);
        
		newSd = Etc::portAccept(serverSd, newSockAddr);

		Player::setUPort(std::stoi(Etc::portIn(newSd))); /// three-way handshake
		Etc::portOut(newSd, std::to_string(Player::getUPort()));
		std::string sPort = Etc::portIn(newSd); 

		Player::setUAddress(Etc::portIn(newSd));
		Etc::portOut(newSd, Player::getUAddress());
		std::string sAddress = Etc::portIn(newSd);
		
		Etc::errChk(-1 + (sPort == std::to_string(Player::getUPort()) && sAddress == Player::getUAddress()), "Something has gone terribly wrong!");
        
        Etc::stopDots(t);
	}
	
    sockaddr_in sendSockAddr;
    Etc::clientSetup(Player::getUPort(), Player::getUAddress(), clientSd, sendSockAddr);
	if (argc == 5) { /// send iPort iAddress
    //TODO: If the port connection fails (ex. desired other player is not running), this should fail gracefully.
		Etc::portConnect(clientSd, sendSockAddr);
		
		Etc::portOut(clientSd, std::to_string(Player::getIPort())); /// three-way handshake
		std::string sPort = Etc::portIn(clientSd);
		Etc::portOut(clientSd, sPort);
		
		Etc::portOut(clientSd, Player::getIAddress());
		std::string sAddress = Etc::portIn(clientSd);
		Etc::portOut(clientSd, sAddress);
		
		Etc::errChk(-1 + (sPort == std::to_string(Player::getIPort()) && sAddress == Player::getIAddress()), "Something has gone terribly wrong!");
	}
	
	/* Credit to GitHub user vmrob for simple nonblocking read from std::cin
	https://gist.github.com/vmrob/ff20420a20c59b5a98a1 */
	std::future<void> portConnect = std::async(std::launch::async, Etc::portConnect, clientSd, sendSockAddr); // TODO: Redundant initialization,
	std::future<int> portAccept = std::async(std::launch::async, Etc::portAccept, serverSd, newSockAddr); // since this is repeated in the loop?
	
	while (newSd == -1) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "." << std::flush;
		
		if (portConnect.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			portConnect = std::async(std::launch::async, Etc::portConnect, clientSd, sendSockAddr);
		}
	
		if (portAccept.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			newSd = portAccept.get();
			portAccept = std::async(std::launch::async, Etc::portAccept, serverSd, newSockAddr);
		}
	}

	Etc::consoleOut("\nRemote connection established!\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::cout << "iport: " << Player::getIPort() << " iaddress: " << Player::getIAddress() << std::endl;
	std::cout << "uport: " << Player::getUPort() << " uaddress: " << Player::getUAddress() << std::endl;
    std::cout << "clientSd: " << Player::getClientSd() << " serverSd: " << Player::getServerSd() << " newSd: " << Player::getNewSd() << std::endl;
    std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
	Etc::consoleOut("Welcome to Sea Battle, a game by Team Cookies\n");

    Etc::consoleOut("Field: \n");
	Board field; // Player's board
	field.initBoardSea();
	field.initBoardShips(); // Field knows player ship locations
	field.printBoard();

    Etc::consoleOut("Map: \n");
	Board map; // Enemy's board
	map.initBoardSea();
	//map.initBoardShips(); // Map does not know enemy ship locations
	map.printBoard();

    int turn = 0;
    while (true) {
        turn++;
        std::string mapTargetCoords;
        std::string fieldTargetCoords;

        Etc::consoleOut("launch an attack on coordinate: "); //cout-cin combo creates a GUI facade
        std::future<std::string> consoleIn = std::async(std::launch::async, Etc::consoleIn); // TODO: Redundant async initialization,
        std::future<std::string> fieldTargetCoordsIn = std::async(std::launch::async, Etc::portIn, newSd); // when updated in the loop?
        std::future<void> mapTargetcoordsOut = std::async(std::launch::async, Etc::portOut, clientSd, mapTargetCoords);

        bool warning = false;
        while (mapTargetCoords.empty() || fieldTargetCoords.empty()) {
            if (!mapTargetCoords.empty()) {
                if (warning == false) {
                    Etc::consoleOut("waiting for opponent to make a move.");
                    warning = true;
                }
                Etc::consoleOut(".");
            }
            else if (!fieldTargetCoords.empty()) {
                if (warning == false) {
                    Etc::consoleOut("\nopponent is waiting for you to make a move!\nlaunch an attack on coordinate: ");
                    warning = true;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(1)); // One move allowed per second

            if (consoleIn.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                mapTargetCoords = consoleIn.get();
                consoleIn = std::async(std::launch::async, Etc::consoleIn);
                mapTargetcoordsOut = std::async(std::launch::async, Etc::portOut, clientSd, mapTargetCoords); // Is using future on this line necessary if not blocking program?
            }
            if (fieldTargetCoordsIn.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                fieldTargetCoords = fieldTargetCoordsIn.get();
                fieldTargetCoordsIn = std::async(std::launch::async, Etc::portIn, newSd);
            }
        }

        Etc::consoleOut("\nincoming transmission...\n");

        std::string fieldTargetStatus = field.attackBoard(field.requestTranslator(fieldTargetCoords[0]), field.requestTranslator(fieldTargetCoords[1])); // Assumes coords are at pos 0 and 1
        std::string mapTargetStatus;
        std::future<void> fieldTargetStatusOut = std::async(std::launch::async, Etc::portOut, clientSd, fieldTargetStatus); // TODO: Redundant initialization if updated in loop?
        std::future<std::string> mapTargetStatusOut = std::async(std::launch::async, Etc::portIn, newSd);
        
        while (mapTargetStatus.empty()) {
            //Alternatively to threading, we could implement style of Aloha which waits for a random amount of time to send, and listen for the rest of the time.
            std::this_thread::sleep_for(std::chrono::seconds(1)); // One move allowed per second
            //std::cout << "send: " << send << "recieve: " << recieve << std::endl;
            
            if (fieldTargetStatusOut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) { // TODO: is this necessary, since write does not block the program?
                fieldTargetStatusOut = std::async(std::launch::async, Etc::portOut, clientSd, fieldTargetStatus);
            }
            if (mapTargetStatusOut.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                mapTargetStatus = mapTargetStatusOut.get();
                mapTargetStatusOut = std::async(std::launch::async, Etc::portIn, newSd);
            }
        }

        Etc::consoleOut(fieldTargetCoords + " is being attacked... " + fieldTargetStatus);
        Etc::consoleOut("launching attack on " + mapTargetCoords + "... " + mapTargetStatus + "\n");
	    map.setBoard(map.requestTranslator(mapTargetCoords[0]), map.requestTranslator(mapTargetCoords[1]), map.responseTranslator(mapTargetStatus[0])); // Assumes coords and status are at pos 0 and 1
        Etc::consoleOut("Field: \n");
        field.printBoard(); // TODO merge printBoard statements to print side by side
        Etc::consoleOut("Map: \n");
        map.printBoard();

        //std::cout << std::flush << std::endl; // Program hangs right here and I have no idea what is wrong with it. Bth players mst press enter to proceed.
        std::cout << "PRESS ENTER TO CONTINUE TO NEXT TURN" << std::endl; //I htink it may have to do with the lingering Etc::consoleIn async function.
        //Symptoms so far: Sometimes the "turn sequence" will begin immediately upon pressing enter. It does this for each player, individually.
        //Other times, it will only begin , and for both players at the same time, when BOTH players have pressed enter.
        //Again, this happens imediately, without waiting for 1 second. (perhaps the peers are actuall sending a write??)
    }
    // TODO: win conditions
    std::cerr << "Until updated, this program should never reach here.";

    close(Player::getServerSd());
    close(Player::getNewSd());
    close(Player::getClientSd());
    exit(0);
    return 0;
}
