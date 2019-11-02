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
using namespace std; //Duplicated comments will be listed in Server.cpp.

char serverBoard[BOARD_SIZE][BOARD_SIZE]; //Initialize empty server board
//char clientBoard[BOARD_SIZE][BOARD_SIZE]; //Initialize empty client board

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
	//TODO: Move all board functions into it's own cpp module.
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
	initBoardSea(serverBoard);
	//initBoardShips(serverBoard); //The client does not know the server's ship locations!!
	printBoard(serverBoard);

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
	
	setBoard(serverBoard, to_int(readbuf[0]), to_int(readbuf[1]), readbuf[2]);
	printBoard(serverBoard);
	
	/*Your retriever should exit after receiving the response.*/
	goto send;
	close(clientSd);
	exit(0);
}