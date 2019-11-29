/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Team Cookies

	Build: g++ -o player.out Player.cpp -pthread
	Usage: ./player.out [iport] [uport] [uaddress]
    Example: ./player.out 6932 2396 localhost
**/

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
#define DEFAULT_NAME "localhost"
#define DEFAULT_PORT 6932

// Setup: Connects one peer to another. Each peer has both IP and port of both peers.

// Turn: Each peer will listen for requests and user input, whichever comes first.
// cout << "Make a move!" << endl;
// When a user input comes in (eg. a1), the sender forwards it as a request.
// The reciever will mark its board and send a response (eg. Hit!).
// The sender will mark its map and wait for requests.
// cout << "Waiting for the enemy battleship." << endl;
// When a request comes in (eg. a1), the sender will mark its board and send a response (eg. Hit!).
// Once it recognizes that a user input and request has been completed, it will print both boards.

void Player::playerConnect( int clientSd, sockaddr_in sendSockAddr ) {
	connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) );
}

int Player::playerAccept( int serverSd, sockaddr_in newSockAddr ) {
    socklen_t newSockAddrSize = sizeof(newSockAddr); // TODO: Redundant storage,
	int newSd = accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize ); // could be put in-line here
    return newSd;
}

std::string Player::playerCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

std::string Player::playerRead(int newSd) {
    char response[1024]; //TODO: There's a bug that limits char sending to 7 max chars. not sure why
    bzero(response, sizeof(response)); // TODO: is this necessary?
    read(newSd, response, sizeof(response));
    return response;
}

void Player::playerWrite( int clientSd, std::string requestString ) {
    const char * request = requestString.c_str(); // TODO: Redundant storage
	write(clientSd, request, sizeof(request)); //single write
}

int Player::main( int argc, char *argv[] ) {
    ///0: Set up the remote connection

    int iport = std::stoi(argv[1]);
    int uport = std::stoi(argv[2]);
    std::string uaddress = argv[3];
    
    /** //TODO: Proper argument handling
	std::string server_name;

    if (argc == 1) {
        cerr << "TODO: Launch Server using default ip and port";
    }
    else if (argc == 2) {
        cout << "One argument should be port, and should launch the server at port";
    }
    else if (argc == 3) {
        cout << "Two arguments should beport and ip, and should launch client connecting to ip at port.";
    }
    else{
        cout << "./player.out [iport] [uport] [uaddress]" << argc;
    }**/

    std::cout << "\nPreparing local resources..." << std::flush; // cout must flush for futures compatibility
    struct hostent* host = gethostbyname( uaddress.c_str() );
    sockaddr_in sendSockAddr;
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET;
    sendSockAddr.sin_addr.s_addr =
      inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( uport );
	int clientSd = Etc::errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Opening stream socket");
    //	Etc::errChk(connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) ), "Error: Socket invalid");

    sockaddr_in acceptSockAddr;
    bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
    acceptSockAddr.sin_family      = AF_INET;
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port        = htons( iport );
    int serverSd = Etc::errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Stream-oriented socket failed to open.");
    const int on = 1;
    Etc::errChk(setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, 
                sizeof( int ) ), "Error: SO_REUSEADDR failed to set.");
    Etc::errChk(bind( serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ) ), "Error: Socket failed to bind.");
    Etc::errChk(listen( serverSd, 5 ), "Error: Operating system failed to listen.");
	sockaddr_in newSockAddr;
    int newSd; // = Etc::errChk(accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize ), "Error: Socket failed to accept.");
    std::cout << "\nLocal resources prepared!" << std::flush;

	std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "\nEstablishing remote connection..." << std::flush; // cout does not work with futures until output sequence is flushed
    /* Credit to GitHub user vmrob for simple nonblocking read from std::cin
    https://gist.github.com/vmrob/ff20420a20c59b5a98a1 */
    std::future<void> playerConnect = std::async(std::launch::async, Player::playerConnect, clientSd, sendSockAddr); // TODO: Redundant initialization,
    std::future<int> playerAccept = std::async(std::launch::async, Player::playerAccept, serverSd, newSockAddr); // since this is repeated in the loop?
    
    while (newSd == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "." << std::flush;
        
        if (playerConnect.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            playerConnect = std::async(std::launch::async, Player::playerConnect, clientSd, sendSockAddr);
        }

        if (playerAccept.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            newSd = playerAccept.get();
            playerAccept = std::async(std::launch::async, Player::playerAccept, serverSd, newSockAddr);
        }
    }

    std::cout << "\nRemote connection established!" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "iport: " << iport << std::endl;
	std::cout << "uaddress: " << uaddress << " uport: " << uport << std::endl;
    std::cout << "clientSd: " << clientSd << "newSd: " << newSd << std::endl;
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
        std::future<std::string> playerCin = std::async(std::launch::async, Player::playerCin); // TODO: Redundant initialization,
        std::future<std::string> playerRead = std::async(std::launch::async, Player::playerRead, newSd); // when updated in the loop?
        std::future<void> playerWrite = std::async(std::launch::async, Player::playerWrite, clientSd, mapTargetCoords);

        while (mapTargetCoords.empty() || fieldTargetCoords.empty()) {
            //if (!mapTargetCoords.empty()) { std::cout << " Waiting for opponent to make a move. " << std::flush; } //TODO: Fancify this ui
            //else if (!fieldTargetCoords.empty()) { std::cout << "Opponent is waiting for you to make a move!" << std::flush; }
            //else { std::cout << "Debugging"; }
            std::this_thread::sleep_for(std::chrono::seconds(1)); // One move allowed per second
            //std::cout << "." << std::flush;

            if (playerCin.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                mapTargetCoords = playerCin.get();
                playerCin = std::async(std::launch::async, Player::playerCin);

                //const char * mapTargetCoordsPointer = mapTargetCoords.c_str();
	            //write(clientSd, mapTargetCoordsPointer, sizeof(mapTargetCoordsPointer)); //single write
                playerWrite = std::async(std::launch::async, Player::playerWrite, clientSd, mapTargetCoords); // Is using future on this line necessary if not blocking program
            }
            if (playerRead.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                fieldTargetCoords = playerRead.get();
                playerRead = std::async(std::launch::async, Player::playerRead, newSd);
            }
        }

        std::string fieldTargetStatus = field.attackBoard(field.requestTranslator(fieldTargetCoords[0]), field.requestTranslator(fieldTargetCoords[1])); // Assumes coords are at pos 0 and 1
        std::string mapTargetStatus;
        std::future<void> clientWrite = std::async(std::launch::async, Player::playerWrite, clientSd, fieldTargetStatus); // TODO: Redundant initialization,
        std::future<std::string> serverRead = std::async(std::launch::async, Player::playerRead, newSd); // when updated in the loop?

        while (mapTargetStatus.empty()) {
            //Alternatively to threading, we could implement style of Aloha which waits for a random amount of time to send, and listen for the rest of the time.
            std::this_thread::sleep_for(std::chrono::seconds(1)); // One move allowed per second
            //std::cout << "." << std::flush;
            //std::cout << "send: " << send << "recieve: " << recieve << std::endl;

            if (clientWrite.wait_for(std::chrono::seconds(0)) == std::future_status::ready) { // TODO: is this necessary, since write does not block the program?
                //const char * fieldTargetStatusPointer = fieldTargetStatus.c_str();
                //write(clientSd, fieldTargetStatusPointer, sizeof(fieldTargetStatusPointer));
                clientWrite = std::async(std::launch::async, Player::playerWrite, clientSd, fieldTargetStatus);
            }
            if (serverRead.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                mapTargetStatus = serverRead.get();
                serverRead = std::async(std::launch::async, Player::playerRead, newSd);
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
        std::cout << "PRESS ENTER TO CONTINUE TO NEXT TURN" << std::endl;
    }
// TODO: win conditions
    std::cerr << "Until updated, this program should never reach here.";
    //TODO: Socket termination
}
