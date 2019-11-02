/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Lloyd Deng

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
#define DEFAULT_FILESYSTEM "filesystem/server"
#define DEFAULT_FILE "filesystem/server/file.html"
#define INT_MAX 2147483647
#define BOARD_SIZE 6
#define SHIP 'O' //○
#define HIT 'X' //●
#define SEA '-' //□
#define MISS '+' //■

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
using namespace std;

char serverBoard[BOARD_SIZE][BOARD_SIZE]; //Initialize empty server board
char clientBoard[BOARD_SIZE][BOARD_SIZE]; //Initialize empty client board

char getBoard(char (&board) [BOARD_SIZE][BOARD_SIZE], int x, int y){
	return board[x][y];
}

void setBoard(char (&board) [BOARD_SIZE][BOARD_SIZE], int x, int y, char c){
	serverBoard[x][y] = c;
}

void printBoard(char (&board) [BOARD_SIZE][BOARD_SIZE]){
	std::cout << "player:" << endl;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::cout << getBoard(serverBoard, i, j) << " ";
		};
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void initBoardShips(char (&board) [BOARD_SIZE][BOARD_SIZE]){//TODO: Place ships based on randomly generated list of coordinates?
	//setBoard(serverBoard, 0, 3, MISS);
	setBoard(serverBoard, 1, 0, SHIP);
	setBoard(serverBoard, 1, 1, SHIP);
	setBoard(serverBoard, 1, 2, SHIP);
	setBoard(serverBoard, 1, 3, SHIP); //setBoard(serverBoard, 1, 3, HIT);
	//setBoard(serverBoard, 1, 4, MISS);
	//setBoard(serverBoard, 2, 3, MISS);
	setBoard(serverBoard, 3, 4, SHIP);
	setBoard(serverBoard, 4, 0, SHIP);
	setBoard(serverBoard, 4, 1, SHIP); //setBoard(serverBoard, 4, 1, HIT);
	setBoard(serverBoard, 4, 4, SHIP);
	setBoard(serverBoard, 5, 0, SHIP);
	setBoard(serverBoard, 5, 1, SHIP); //setBoard(serverBoard, 5, 1, HIT);
	setBoard(serverBoard, 5, 4, SHIP);
}

void initBoardSea(char (&board) [BOARD_SIZE][BOARD_SIZE]){
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			setBoard(serverBoard, i, j, SEA);
		};
	}
}

int errChk(int errVal, string errMsg){
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
		
	initBoardSea(serverBoard);
	initBoardShips(serverBoard);
	printBoard(serverBoard);
	std::cout << "Welcome to Sea Battle, a game by Team Cookies" << std::endl;

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

	accept:
	/*Your server waits for a connection and an HTTP GET request*/
	char readbuf[1024];
	bzero(readbuf, sizeof(readbuf));
	read(newSd, readbuf, sizeof(readbuf));
	std::cout << "reading: " << readbuf << std::endl;
	
	string swritebuf = "Server says: OK!";
	const char * writebuf = swritebuf.c_str();
	write(newSd, writebuf, strlen(writebuf));
	std::cout << "writing: " << swritebuf << std::endl;

	/*After you handle the request, your server should return to waiting for the next request.*/
	goto accept;
	close(newSd);
	exit(0);
} 
