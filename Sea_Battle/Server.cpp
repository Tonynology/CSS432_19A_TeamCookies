/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Team Cookies

	Build: g++ -o server.out Server.cpp
	
	Usage: ./server.out [port] [n]
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
#include <string.h>
#include "Board.h"
#define DEFAULT_NAME "localhost"
#define DEFAULT_PORT 6932
#define DEFAULT_N 10

void printEndl(){
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
}

void printWlcm(){
	std::cout << "Welcome to Sea Battle v1.4 beta, a game by Team Cookies\nThis is the server application recieves attack requests from the client and manages the field and responds to the client\nWatch this window while interacting with the client to learn its networking protocol implementation\n" << std::endl;
}

int errChk(int errVal, std::string errMsg){
	if (errVal < 0) { // errVals under 0 will print errMsg and then exit the program with errVal.
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal; // errVals over 0 will return the errVal without printing or exiting.
}

int main(int argc, char const *argv[]){	
	int port, n;
	int server_port;
	std::string server_name;
	if (argc == 1){
		port = DEFAULT_PORT;
		n = DEFAULT_N;
		server_name = DEFAULT_NAME;
		server_port = DEFAULT_PORT + 1;
	}
	else if (argc == 2){
		port = std::stoi(argv[1]);
		n = DEFAULT_N;
		server_name = argv[1];
		server_port = DEFAULT_PORT;
	}
	else if (argc == 3){
		port = std::stoi(argv[1]);
		n = std::stoi(argv[2]);
		server_name = argv[1];
		server_port = std::stoi(argv[3]);
	}
	else{
		errChk(-1, "Usage: ./server.out [port] [n]");
	}
    sockaddr_in acceptSockAddr;
    bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
    acceptSockAddr.sin_family      = AF_INET;
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port        = htons( port );
    int serverSd = errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Stream-oriented socket failed to open.");
    const int on = 1;
    errChk(setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, 
                sizeof( int ) ), "Error: SO_REUSEADDR failed to set.");
    errChk(bind( serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ) ), "Error: Socket failed to bind.");
    errChk(listen( serverSd, n ), "Error: Operating system failed to listen.");
	sockaddr_in newSockAddr;
	socklen_t newSockAddrSize = sizeof( newSockAddr );
	int newSd = errChk(accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize ), "Error: Socket failed to accept.");
	struct hostent* host = gethostbyname( server_name.c_str() );
    sockaddr_in sendSockAddr;
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET;
    sendSockAddr.sin_addr.s_addr =
      inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( server_port );
	int clientSd = errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Opening stream socket");
	errChk(connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) ), "Error: Socket invalid");
	
	start:	/* Start */
	///1: Displays welcome to User; Initialize the field
	std::cout << "Port: " << port << " n: " << n << std::endl;
	std::cout << "IP: " << server_name << " Port: " << server_port << std::endl;
	printEndl();
	printWlcm();
	int turn = 0;
	Board field;
	field.initBoardSea();
	field.initBoardShips(); //Field knows player ship locations
	field.printBoard();
	Board map;
	map.initBoardSea();
	//map.initBoardShips(); //Map does not know enemy ship locations
	map.printBoard();

	loop: /* Loop */
	///5: Reads request from client
	turn++;
	char request[1024];
	bzero(request, sizeof(request));
	read(newSd, request, sizeof(request)); //waiting on CLIENT
	std::cout << "request: " << request << std::endl;
	
	///6: Converts request[0], request[1] to int coordinates; marks field; stores response
	std::string responses = field.attackBoard(field.requestTranslator(request[0]), field.requestTranslator(request[1]));
	
	///7: Writes response to Client
	const char * response = responses.c_str();
	write(newSd, response, strlen(response)); //single write
	std::cout << "response: " << response << std::endl;
	
	///10: Displays field to User
	field.printBoard();
	
	loop2:
	turn++;
	std::string requests;
	std::cout << "attacking: "; //cout-cin combo creates a GUI facade
	std::cin >> requests; //waiting on USER
	requests += " is being attacked!";
	const char * request2 = requests.c_str();
	write(clientSd, request2, strlen(request2)); //single write
	//std::cout << "attacking: " << requests << std::endl; //see: "GUI"
	char response2[1024];
	bzero(response2, sizeof(response2));
	read(clientSd, response2, sizeof(response2)); //waiting on SERVER
	std::cout << "turn " << turn << ": " << response2 << std::endl;
	map.setBoard(map.requestTranslator(requests[0]), map.requestTranslator(requests[1]), map.responseTranslator(response2[0]));
	map.printBoard();
	
	///12: if end go to 14; if loop go to 5
	if (turn > 1024) goto end;
	else goto loop;
	
	end: /* End */
	///14: Close the socket and exit the game.
	std::cout << "Game Over!";
	close(newSd);
	close(clientSd);
	exit(0);
} 
