/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Team Cookies

This is the golden master release of Sea Battle Alpha.

	Build: g++ -o server.out Server.cpp -pthread
	
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
#define DEFAULT_PORT 6932
#define DEFAULT_N 10
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"
//#define INT_MAX 2147483647
#define BOARD_SIZE 6
#define SHIP 'O' //○
#define HIT 'X' //●
#define SEA '-' //□
#define MISS '+' //■
#define SERVER_WELCOME "Welcome to Sea Battle v1.0 alpha, a game by Team Cookies\nThis is the server application which manages the game board. Keep this window open while playing the game through the client to see the networking between the two applications.\n"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include "Board.h"

using namespace std; //TODO: Remove namespace

int to_int(char c){
	return c - 48; //convert from 
}

int errChk(int errVal, string errMsg){
	//errVals under 0 will result in printing errMsg and exiting the program.
	//errVals over 0 will return the errVal.
	if (errVal < 0) {
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal;
}


int main(int argc, char const *argv[])
{	
	int port, n;
	if (argc == 1){ //No arguments: Prompt the user
		port = DEFAULT_PORT;
		n = DEFAULT_N;
	}
	
	else if (argc == 2){ //One argument: Assume user input = port
		port = stoi(argv[1]);
		n = DEFAULT_N;
	}
	
	else if (argc == 3){ //Two arguments: Assume user input = port, n
		port = stoi(argv[1]);
		n = stoi(argv[2]);
	}
	
	else{ //None of the above: Exit
		errChk(-1, "Usage: ./server.out [port] [n]");
	}
	
	/*Your server only needs to respond to HTTP GET request.*/
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
	
	//User interactions begin here.
	std::cout << "Port: " << port << " n: " << n << std::endl << SERVER_WELCOME;
	Board serverBoard;
	//Board clientBoard;
	serverBoard.initBoardSea();
	serverBoard.initBoardShips();
	serverBoard.printBoard();

	accept:
	/*Your server waits for a connection and an HTTP GET request*/
	char readbuf[1024];
	bzero(readbuf, sizeof(readbuf));
	read(newSd, readbuf, sizeof(readbuf));
	std::cout << "reading: " << readbuf << std::endl;
	
	string swritebuf = serverBoard.attackBoard(readbuf[0], readbuf[1]);
	//swritebuf += serverBoard.getBoard();
	const char * writebuf = swritebuf.c_str();
	write(newSd, writebuf, strlen(writebuf));
	std::cout << "writing: " << swritebuf << std::endl;
	
	serverBoard.printBoard();

	/*After you handle the request, your server should return to waiting for the next request.*/
	goto accept;
	close(newSd);
	exit(0);
} 
