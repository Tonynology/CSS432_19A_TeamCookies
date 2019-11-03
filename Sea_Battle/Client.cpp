/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Team Cookies

	Build: g++ -o client.out Client.cpp
	
	Usage: ./client.out [server_name] [server_port]
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
	std::cout << "Welcome to Sea Battle v1.4 beta, a game by Team Cookies\nThis is the client application which sends attack requests to the server and marks a map based off of its reponse\nPlease enter coordinates formatted as [char][num]<Enter> (no spaces, no brackets)\n" << std::endl;
}

int errChk(int errVal, string errMsg){
	//errVals under 0 will result in printing errMsg and exiting the program; at or over 0 will return the errVal
	if (errVal < 0) {
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal;
}

int main(int argc, char const *argv[]){	
	int server_port;
	int port, n;
	std::string server_name;
	if (argc == 1){
		port = DEFAULT_PORT + 1;
		n = DEFAULT_N;
		server_name = DEFAULT_NAME;
		server_port = DEFAULT_PORT;
	}
	else if (argc == 2){
		port = stoi(argv[1]);
		n = DEFAULT_N;
		server_name = argv[1];
		server_port = DEFAULT_PORT;
	}
	else if (argc == 3){
		port = stoi(argv[1]);
		n = stoi(argv[2]);
		server_name = argv[1];
		server_port = stoi(argv[3]);
	}
	else{
		errChk(-1, "Usage: ./client.out [server_name] [server_port]");
	}
	struct hostent* host = gethostbyname( server_name.c_str() );
    sockaddr_in sendSockAddr;
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET;
    sendSockAddr.sin_addr.s_addr =
      inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( server_port );
	int clientSd = errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Opening stream socket");
	errChk(connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) ), "Error: Socket invalid");
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
	
	start: /* Start */
	std::cout << "IP: " << server_name << " Port: " << server_port << std::endl;
	std::cout << "Port: " << port << " n: " << n << std::endl;
	printEndl();
	printWlcm();
	int turn = 0;
	Board map;
	map.initBoardSea();
	//map.initBoardShips(); //Map does not know enemy ship locations
	map.printBoard();
	Board field;
	field.initBoardSea();
	field.initBoardShips(); //Field knows player ship locations
	field.printBoard();

	loop: /* Loop */
	///3: Reads request from User; stores request
	turn++;
	std::string requests;
	std::cout << "attacking: "; //cout-cin combo creates a GUI facade
	std::cin >> requests; //waiting on USER
	requests += " is being attacked!";
	
	///4: Writes request to server
	const char * request = requests.c_str();
	write(clientSd, request, strlen(request)); //single write
	//std::cout << "attacking: " << requests << std::endl; //see: "GUI"
	
	///8: Reads response from Server
	char response[1024];
	bzero(response, sizeof(response));
	read(clientSd, response, sizeof(response)); //waiting on SERVER
	std::cout << "turn " << turn << ": " << response << std::endl;
	
	///9: Converts response[0] to status; marks map
	map.setBoard(map.requestTranslator(requests[0]), map.requestTranslator(requests[1]), map.responseTranslator(response[0]));
	
	///11: Displays map to User
	map.printBoard();
	
	loop2:
	turn++;
	char request2[1024];
	bzero(request2, sizeof(request2));
	read(newSd, request2, sizeof(request2)); //waiting on CLIENT
	std::cout << "request2: " << request2 << std::endl;
	string responses = field.attackBoard(field.requestTranslator(request2[0]), field.requestTranslator(request2[1]));
	const char * response2 = responses.c_str();
	write(newSd, response2, strlen(response2)); //single write
	std::cout << "response2: " << response2 << std::endl;
	field.printBoard();
	
	///13: if end go to 15; if loop go to 3
	if (turn > 1024) goto end;
	else goto loop;

	end: /* End */
	///15: Close the socket and exit the game.
	std::cout << "Game Over!";
	close(clientSd);
	close(newSd);
	exit(0);
}
