/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Team Cookies

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
using namespace std; //TODO: Remove namespace

char serverBoard[BOARD_SIZE][BOARD_SIZE]; //Initialize empty server board
//char clientBoard[BOARD_SIZE][BOARD_SIZE]; //Initialize empty client board

int errChk(int errVal, string errMsg){
	//errVals under 0 will result in printing errMsg and exiting the program.
	//errVals over 0 will return the errVal.
	if (errVal < 0) {
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal;
}

int atkHlp(char c){
	//TODO: Must be a better way of doing this.
	//Maybe switch statement? Maybe key-value dict map?
	//Also, should support the full alphabet, not just six, since we don't have this hard coded.
	
	//TODO: Split this into a converter board.
	if (c == 'a'){
		return 0;
	}
	else if (c == 'b'){
		return 1;
	}
	else if (c == 'c'){
		return 2;
	}
	else if (c == 'd'){
		return 3;
	}
	else if (c == 'e'){
		return 4;
	}
	else if (c == 'f'){
		return 5;
	}
	else if (c == 'b'){
		return 6;
	}
	else if (c == '1'){
		return 0;
	}
	else if (c == '2'){
		return 1;
	}
	else if (c == '3'){
		return 2;
	}
	else if (c == '4'){
		return 3;
	}
	else if (c == '5'){
		return 4;
	}
	else if (c == '6'){
		return 5;
	}
	else if (c == '7'){
		return 6;
	}
	else{ //TODO: Do this with errChk
		errChk(-1, "invalid input: " + to_string(c)); 
	}
}

char getBoard(char (&board) [BOARD_SIZE][BOARD_SIZE], int x, int y){
	return serverBoard[x][y];
}

std::string getBoard(char (&board) [BOARD_SIZE][BOARD_SIZE]){
	//Overloaded constructor of getBoard which returns the entire board as a string.
	//TODO: This is very similar to printBoard. How can we combine them?
	//TODO: Something in this method is causing a memory leak after ctrl c exiting the program??
	string s = "  ";
	for (int k = 1; k <= BOARD_SIZE; k++) {
		s += std::to_string(k);
		s += " ";
	}
	s += "\n";

	for (int i = 0; i < BOARD_SIZE; i++){
		s += ALPHABET[i % BOARD_SIZE];
		s += " ";

		for (int j = 0; j < BOARD_SIZE; j++){
			s += getBoard(serverBoard, i, j);
			s += " ";
		}
		s += "\n";
	}
	s += "\n";
	return s;
}

void setBoard(char (&board) [BOARD_SIZE][BOARD_SIZE], int x, int y, char c){
	serverBoard[x][y] = c;
}

std::string attackBoard(char a, char b){
	//TODO: Support more formats than just [char][num]
	int x = atkHlp(a);
	int y = atkHlp(b);
	char c = getBoard(serverBoard, x, y);
	
	if (c == SHIP){
		setBoard(serverBoard, x, y, HIT);
		return "hit!\n";
	}
	else if (c == HIT){
		setBoard(serverBoard, x, y, HIT);
		return "hey, you already hit this spot...\n";
	}
	else if (c == SEA){
		setBoard(serverBoard, x, y, MISS);
		return "miss!\n";
	}
	else if (c == MISS){
		setBoard(serverBoard, x, y, MISS);
		return "hey, you already missed this spot...\n";
	}
}

void printBoard(char (&board) [BOARD_SIZE][BOARD_SIZE]){
	std::cout << "  ";
	for (int k = 1; k <= BOARD_SIZE; k++) {
		cout << std::to_string(k) << " ";
	}
	std::cout << std::endl;
	
	for (int i = 0; i < BOARD_SIZE; i++) {
		std::cout << ALPHABET[i % BOARD_SIZE] << " ";
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::cout << getBoard(board, i, j) << " ";
		};
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void initBoardShips(char (&board) [BOARD_SIZE][BOARD_SIZE]){//TODO: Place ships based on randomly generated list of coordinates?
	//setBoard(serverBoard, 0, 3, MISS); //To be entered by user
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
	
	//Initialization here
	initBoardSea(serverBoard);
	initBoardShips(serverBoard);
	std::cout << "Welcome to Sea Battle, a game by Team Cookies" << std::endl;
	std::cout << "This is the server application, which stores the game board and calculates all hits and misses. Keep this window open, and run the client, to play the game using that window." << std::endl;
	std::cout << "Enjoy the game!" << std::endl;
	printBoard(serverBoard);

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
	
	string swritebuf = attackBoard(readbuf[0], readbuf[1]);
	swritebuf += getBoard(serverBoard);
	const char * writebuf = swritebuf.c_str();
	write(newSd, writebuf, strlen(writebuf));
	std::cout << "writing: " << swritebuf << std::endl;

	/*After you handle the request, your server should return to waiting for the next request.*/
	goto accept;
	close(newSd);
	exit(0);
} 
