#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <iostream>
#include <string>
#include <future>

#include "Player.h"
#include "Board.h"
#include "Etc.h"

#define DEFAULT_IPORT 6932
#define DEFAULT_IADDRESS "localhost"
#define DEFAULT_UPORT 6933 // port and address cannot both be the same
#define DEFAULT_UADDRESS "localhost"

int iPort = DEFAULT_IPORT;
std::string iAddress = DEFAULT_IADDRESS;
int uPort = DEFAULT_UPORT;
std::string uAddress = DEFAULT_UADDRESS;

int Player::getIPort( ) { return iPort; }
std::string Player::getIAddress( ) { return iAddress; }
int Player::getUPort( ) { return uPort; }
std::string Player::getUAddress( ) { return uAddress; }

void Player::setIPort( int i ) { iPort = i; }
void Player::setIAddress( std::string i ) { iAddress = i; }
void Player::setUPort( int u ) { uPort = u; }
void Player::setUAddress( std::string u ) { uAddress = u; }

int main( int argc, char *argv[] ) {
	std::cout << "\nPreparing local resources..." << std::flush;
	
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
    else if (argc == 5){ // four args: second player informs challenger
		Player::setIPort(std::stoi(argv[1]));
		Player::setIAddress(argv[2]);
		Player::setUPort(std::stoi(argv[3]));
		Player::setUAddress(argv[4]);
	}
	else {
		Etc::errChk(-1, "Usage: ./player.out [iport] [uport] [uaddress]");
	}
	
	std::cout << "\nLocal resources prepared!" << std::flush;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "\nEstablishing remote connection..." << std::flush; // cout does not work with futures until output sequence is flushed
	
	/* server code */
    sockaddr_in acceptSockAddr;
    bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
    acceptSockAddr.sin_family      = AF_INET;
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port        = htons( Player::getIPort() );
    int serverSd = Etc::errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Stream-oriented socket failed to open.");
    const int on = 1;
    Etc::errChk(setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, 
                sizeof( int ) ), "Error: SO_REUSEADDR failed to set.");
    Etc::errChk(bind( serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ) ), "Error: Socket failed to bind.");
    Etc::errChk(listen( serverSd, 5 ), "Error: Operating system failed to listen.");
	sockaddr_in newSockAddr;
    int newSd = -1; // = Etc::errChk(accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize ), "Error: Socket failed to accept.");
	if (argc == 3){ /// read uPort uAddress
		newSd = Etc::portAccept(serverSd, newSockAddr);
		//TODO: async the appearing "..."
		Player::setUPort(std::stoi(Etc::portIn(newSd))); /// three-way handshake
		Etc::portOut(newSd, std::to_string(Player::getUPort()));
		std::string sPort = Etc::portIn(newSd); 

		Player::setUAddress(Etc::portIn(newSd));
		Etc::portOut(newSd, Player::getUAddress());
		std::string sAddress = Etc::portIn(newSd);
		
		Etc::errChk(-1 + (sPort == std::to_string(Player::getUPort()) && sAddress == Player::getUAddress()), "Something has gone terribly wrong!");
	}
	
	/* client code */
    struct hostent* host = gethostbyname( Player::getUAddress().c_str() );
    sockaddr_in sendSockAddr;
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET;
    sendSockAddr.sin_addr.s_addr =
      inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( Player::getUPort() );
	int clientSd = Etc::errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Opening stream socket");
    //	Etc::errChk(connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) ), "Error: Socket invalid");
	if (argc == 5){ /// send iPort iAddress
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

	std::cout << "\nRemote connection established!" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "iport: " << Player::getIPort() << " iaddress: " << Player::getIAddress() << std::endl;
	std::cout << "uport: " << Player::getUPort() << " uaddress: " << Player::getUAddress() << std::endl;
    std::cout << "clientSd: " << clientSd << " serverSd: " << serverSd << " newSd: " << newSd << std::endl;
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
	std::cout << "Welcome to Sea Battle, a game by Team Cookies\n" << std::endl;

    std::cout << "Field: " << std::endl;
	Board field; // Player's board
	field.initBoardSea();
	field.initBoardShips(); // Field knows player ship locations
	field.printBoard();

    std::cout << "Map: " << std::endl;
	Board map; // Enemy's board
	map.initBoardSea();
	//map.initBoardShips(); // Map does not know enemy ship locations
	map.printBoard();

    int turn = 0;

    while (true) {
        turn++;

        std::cout << "turn " << turn << ". launch an attack on coordinate: " << std::flush; //cout-cin combo creates a GUI facade
        std::string mapTargetCoords;
        std::string fieldTargetCoords;
        std::future<std::string> consoleIn = std::async(std::launch::async, Etc::consoleIn); // TODO: Redundant initialization,
        std::future<std::string> portIn = std::async(std::launch::async, Etc::portIn, newSd); // when updated in the loop?
        std::future<void> portOut = std::async(std::launch::async, Etc::portOut, clientSd, mapTargetCoords);

        while (mapTargetCoords.empty() || fieldTargetCoords.empty()) { /// Until both have been set
            //if (!mapTargetCoords.empty()) { std::cout << " Waiting for opponent to make a move. " << std::flush; } //TODO: Fancify this ui
            //else if (!fieldTargetCoords.empty()) { std::cout << "Opponent is waiting for you to make a move!" << std::flush; }
            //else { std::cout << "Debugging"; }
            std::this_thread::sleep_for(std::chrono::seconds(1)); // One move allowed per second

            if (consoleIn.wait_for(std::chrono::seconds(0)) == std::future_status::ready) { /// Set mapTargetcoords from Player
                mapTargetCoords = consoleIn.get();
                consoleIn = std::async(std::launch::async, Etc::consoleIn);

                //const char * mapTargetCoordsPointer = mapTargetCoords.c_str();
	            //write(clientSd, mapTargetCoordsPointer, sizeof(mapTargetCoordsPointer)); //single write
                portOut = std::async(std::launch::async, Etc::portOut, clientSd, mapTargetCoords); // Is using future on this line necessary if not blocking program
            }
            if (portIn.wait_for(std::chrono::seconds(0)) == std::future_status::ready) { /// Set fieldTargetCoords from port
                fieldTargetCoords = portIn.get();
                portIn = std::async(std::launch::async, Etc::portIn, newSd);
            }
        }
        std::string fieldTargetStatus = field.attackBoard(field.requestTranslator(fieldTargetCoords[0]), field.requestTranslator(fieldTargetCoords[1])); // Assumes coords are at pos 0 and 1
        std::string mapTargetStatus;
        std::future<void> clientWrite = std::async(std::launch::async, Etc::portOut, clientSd, fieldTargetStatus); // TODO: Redundant initialization,
        std::future<std::string> serverRead = std::async(std::launch::async, Etc::portIn, newSd); // when updated in the loop?

        while (mapTargetStatus.empty()) {
            //Alternatively to threading, we could implement style of Aloha which waits for a random amount of time to send, and listen for the rest of the time.
            std::this_thread::sleep_for(std::chrono::seconds(1)); // One move allowed per second
            //std::cout << "send: " << send << "recieve: " << recieve << std::endl;

            if (clientWrite.wait_for(std::chrono::seconds(0)) == std::future_status::ready) { // TODO: is this necessary, since write does not block the program?
                //const char * fieldTargetStatusPointer = fieldTargetStatus.c_str();
                //write(clientSd, fieldTargetStatusPointer, sizeof(fieldTargetStatusPointer));
                clientWrite = std::async(std::launch::async, Etc::portOut, clientSd, fieldTargetStatus);
            }
            if (serverRead.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                mapTargetStatus = serverRead.get();
                serverRead = std::async(std::launch::async, Etc::portIn, newSd);
            }
        }

        std::cout << fieldTargetCoords << " is being attacked... " << fieldTargetStatus;
        std::cout << "launching attack on " << mapTargetCoords << "... " << mapTargetStatus << std::endl;
	    map.setBoard(map.requestTranslator(mapTargetCoords[0]), map.requestTranslator(mapTargetCoords[1]), map.responseTranslator(mapTargetStatus[0])); // Assumes coords and status are at pos 0 and 1
        std::cout << "Field: " << std::endl;
        field.printBoard(); // TODO merge printBoard statements to print side by side
        std::cout << "Map: " << std::endl;
        map.printBoard();
        //std::cout << std::flush << std::endl; // Program hangs right here and I have no idea what is wrong with it. Bth players mst press enter to proceed.
        std::cout << "PRESS ENTER TO CONTINUE TO NEXT TURN" << std::endl; //I htink it may have to do with the lingering Etc::consoleIn async function.
        //Symptoms so far: Sometimes the "turn sequence" will begin immediately upon pressing enter. It does this for each player, individually.
        //Other times, it will only begin , and for both players at the same time, when BOTH players have pressed enter.
        //Again, this happens imediately, without waiting for 1 second. (perhaps the peers are actuall sending a write??)
    }
// TODO: win conditions
    std::cerr << "Until updated, this program should never reach here.";
}
