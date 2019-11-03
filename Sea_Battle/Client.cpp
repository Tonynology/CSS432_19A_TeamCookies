/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Team Cookies

This is the golden master release of Sea Battle Alpha.

	Build: g++ -o client.out Client.cpp
	
	Usage: ./client.out [server_name] [server_path] [server_port]
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
#define DEFAULT_NAME "localhost"
#define DEFAULT_PATH "/"
#define DEFAULT_PORT 6932
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"
//#define INT_MAX 2147483647
#define BOARD_SIZE 6
#define SHIP 'O' //○
#define HIT 'X' //●
#define SEA '-' //□
#define MISS '+' //■
#define CLIENT_WELCOME "Welcome to Sea Battle v1.0 alpha, a game by Team Cookies\nThis is the client application which sends attack requests to the server and draws a game board based off its response. Please enter coordinates formatted by [char][num]<Enter> (no spaces, no brackets) to guess ship locations.\n"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include "Board.h"

using namespace std; //Duplicated comments will be listed in Server.cpp.

int to_int(char c){ //TODO: standardize message transcription. Encryption?
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
	/*Your retriever takes in an input from the command line and parses the server address and file (web page) that is being requested.*/
	int ticker = 0;
	int server_port;
	std::string server_name, server_path;

	if (argc == 1){ //No arguments: Prompt the user
		server_name = DEFAULT_NAME;
		server_path = DEFAULT_PATH;
		server_port = DEFAULT_PORT;
	}
	
	else if (argc == 2){ //One argument: Assume user input = server_name
		server_name = argv[1];
		server_path = DEFAULT_PATH;
		server_port = DEFAULT_PORT;
	}

	else if (argc == 3){ //Two arguments: Assume user input = server_name server_path
		server_name = argv[1];
		server_path = argv[2];
		server_port = DEFAULT_PORT;
	}

	else if (argc == 4){ //Three arguments: Assume user input = server_name server_path server_port
		server_name = argv[1];
		server_path = argv[2];
		server_port = stoi(argv[3]);
	}

	else{ //None of the above: Exit
		errChk(-1, "Usage: ./client.out [server_name] [server_path] [server_port]");
	}

	/*The program then issues a GET request to the server for the requested file.*/
	struct hostent* host = gethostbyname( server_name.c_str() );
    sockaddr_in sendSockAddr;
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET;
    sendSockAddr.sin_addr.s_addr =
      inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( server_port );
	int clientSd = errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Opening stream socket");
	errChk(connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) ), "Error: Socket invalid");
	
	//User interactions begin here.
	std::cout << "IP: " << server_name << " Port: " << server_port << std::endl << CLIENT_WELCOME;
	Board serverBoard;
	//Board clientBoard;
	serverBoard.initBoardSea();
	//initBoardShips(serverBoard); //The client does not know the server's ship locations!!
	serverBoard.printBoard();

	send:
	std::string swritebuf;
	std::cout << "writing: ";
	std::cin >> swritebuf;
	const char * writebuf = swritebuf.c_str();
	write(clientSd, writebuf, strlen(writebuf)); // single write: allocates an nbufs-sized array of data buffers, and thereafter calls write( ) to send this array, (i.e., all data buffers) at once.
	//std::cout << "writing: " << swritebuf << std::endl;
	
	char readbuf[1024];
	bzero(readbuf, sizeof(readbuf));
	read(clientSd, readbuf, sizeof(readbuf));
	std::cout << "reading: " << readbuf << std::endl;
	
	serverBoard.setBoard(to_int(readbuf[0]), to_int(readbuf[1]), readbuf[2]);
	serverBoard.printBoard();
	
	/*Your retriever should exit after receiving the response.*/
	goto send;
	close(clientSd);
	exit(0);
}