/**
CSS 432 AU19
HW2 - Intro to Network Programming
Lloyd Deng

	Build: g++ -o server.out Server.cpp -pthread
	
	Usage: ./server.out [port] [n]

Server.cpp opens a simple server defaulting at port 6932 and ip address localhost,
unless otherwise specified during runtime.
The server stays open, waiting for requests from Client.cpp, or from Safari browser.
It can handle up to n clients at any moment,
and after it is finished serving requests,
it returns to the waiting state until manually killed by the user.

This server can handle HTML files up to 2147483647 characters long, and files are saved at filesystem/server/file.html.
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

char board[BOARD_SIZE][BOARD_SIZE]; //Initialize empty game board

void printBoard(){
	std::cout << "player:" << endl;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::cout << board[i][j] << " ";
		};
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void initBoard(){
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = '-'; //□
		};
	}
	//TODO: Users fill in squares of their choice
	board[0][3] = 'O'; //■
	board[1][0] = 'O';//○
	board[1][1] = 'O';//○
	board[1][2] = 'O';//○
	board[1][2] = 'X';//●
	board[2][3] = '+';//■
	board[3][4] = 'O';//○
	board[4][0] = 'O';//○
	board[4][1] = 'X';//●
	board[4][4] = 'O';//○
	board[5][0] = 'O';//○
	board[5][1] = 'X';//●
	board[5][4] = 'O';//○
	printBoard();
}

int errChk(int errVal, string errMsg){
	if (errVal < 0) {
		std::cout << errMsg << std::endl;
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
		
	initBoard();
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
