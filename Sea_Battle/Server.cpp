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

void *server(void *newSd)
{
	int ticker = 0; //Counter for escaping infinite (INT_MAX) loops
	/*Your server waits for a connection and an HTTP GET request*/
	int clientSd = *(int*)newSd; 
	
	string method, path, protocol, header;
	char readbuf[2];

	while(read(clientSd, readbuf, 1)){
		if (ticker++ > INT_MAX){errChk(-1, "Trapped in method loop!");}
		if (readbuf[0] == ' ' || readbuf[1] == ' ') break;
		method += readbuf;
		bzero(readbuf, sizeof(readbuf));
	}
	
	while(read(clientSd, readbuf, 1)){
		if (ticker++ > INT_MAX){errChk(-1, "Trapped in path loop!");}
		if (readbuf[0] == ' ' || readbuf[1] == ' ') break;
		path += readbuf;
		bzero(readbuf, sizeof(readbuf));
	}
	if (path == "/") path = DEFAULT_FILE;
	else if (path[0] == '/') path = DEFAULT_FILESYSTEM + path;
	
	while(read(clientSd, readbuf, 1)){
		if (ticker++ > INT_MAX){errChk(-1, "Trapped in protocol loop!");}
		if (readbuf[0] == '\r' || readbuf[0] == '\n' || readbuf[1] == '\r' || readbuf[1] == '\n') break;
		protocol += readbuf;
		bzero(readbuf, sizeof(readbuf));
	}
	
	while(read(clientSd, readbuf, 1)){
		if (ticker++ > INT_MAX){errChk(-1, "Trapped in header loop!");}
		if (readbuf[0] == '\r' || readbuf[0] == '\n' || readbuf[1] == '\r' || readbuf[1] == '\n') break;
		header += readbuf;
		bzero(readbuf, sizeof(readbuf));
	}

	std::cout << method << " " << path << " " << protocol << "\r\n" << header << "\r\n";
	
	string swritebuf, s;
	ifstream i;
	i.open(path);
	/*Finally, if your server cannot understand the request, return a 400 Bad Request.*/
	if (path.find("filesystem/server/") == -1 || path.find("%") != -1 || method != "GET"){
		swritebuf += "HTTP/1.0 400 Bad Request\r\n\r\n";
		swritebuf += "\r\n";
		const char * writebuf = swritebuf.c_str();
		write( clientSd, writebuf, strlen(writebuf) ); // single write: allocates an nbufs-sized array of data buffers, and thereafter calls write( ) to send this array, (i.e., all data buffers) at once.
	}
	/*If the request is for a file that is above the directory where your web server is running ( for example, "GET ../../../etc/passwd" ), you should return a 403 Forbidden.*/
	else if (path.find("../") != -1){
		swritebuf += "HTTP/1.0 403 Forbidden\r\n\r\n";
		swritebuf += "\r\n";
		const char * writebuf = swritebuf.c_str();
		write( clientSd, writebuf, strlen(writebuf) );
	}
	/*If the HTTP request is for SecretFile.html then the web server should return a 401 Unauthorized.*/
	else if (path.find("SecretFile.html") != -1){
		swritebuf += "HTTP/1.0 401 Unauthorized\r\n\r\n";
		swritebuf += "\r\n";
		const char * writebuf = swritebuf.c_str();
		write( clientSd, writebuf, strlen(writebuf) );
	}
	/*If the file exists, the server opens the file that is requested and sends it (along with the HTTP 200 OK code, of course).*/
	else if (i.is_open()){
		swritebuf += "HTTP/1.0 200 OK\r\n\r\n";
		while(getline(i,s)){
			if (ticker++ > INT_MAX){errChk(-1, "Trapped in getline loop!");}
			swritebuf += s + "\r\n";		
		}
		i.close();
		swritebuf += "\r\n";
		const char * writebuf = swritebuf.c_str();
		write( clientSd, writebuf, strlen(writebuf) );
	}
	/*If the file requested does not exist, the server should return a 404 Not Found code along with a custom File Not Found page.*/
	else if (!i.is_open()){
		i.open("filesystem/server/notfound.html");
		errChk(i.is_open() - 1, "Error: Opening notfound.html");

		swritebuf += "HTTP/1.0 404 Not Found\r\n\r\n";
		while(getline(i,s)){
			if (ticker++ > INT_MAX){errChk(-1, "Trapped in getline loop!");}
			swritebuf += s + "\r\n";		
		}
		i.close();
		swritebuf += "\r\n";
		const char * writebuf = swritebuf.c_str();
		write( clientSd, writebuf, strlen(writebuf) );
	}
	close(clientSd);
	pthread_exit(NULL);
	return 0;
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
	
	accept:
	int newSd = errChk(accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize ), "Error: Socket failed to accept.");
	
	/*multi-threaded handling*/
	pthread_t thread;
	errChk( pthread_create( &thread, NULL, server, (void*)&newSd ), "Error: Thread failed to create." );
	errChk( pthread_join( thread, NULL ), "Error: Thread failed to join." );
	close(newSd);
	
	/*After you handle the request, your server should return to waiting for the next request.*/
	goto accept;
} 
