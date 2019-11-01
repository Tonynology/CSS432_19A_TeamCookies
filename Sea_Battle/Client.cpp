/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Lloyd Deng

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
#define DEFAULT_PORT 80
#define DEFAULT_FILESYSTEM "filesystem/client"
#define DEFAULT_FILE "filesystem/client/file.html"
#define INT_MAX 2147483647
#define BOARD_SIZE 6

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
using namespace std;

char clientBoard[BOARD_SIZE][BOARD_SIZE]; //Initialize empty client board
char serverBoard[BOARD_SIZE][BOARD_SIZE]; //Initialize empty server board

void printBoard(){
	std::cout << "player:" << endl;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::cout << clientBoard[i][j] << " ";
		};
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void initBoard(){
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			clientBoard[i][j] = '-'; //□
		};
	}
	//TODO: Client randomly generated ships
	//clientBoard[0][3] = '+';//■
	clientBoard[1][0] = 'O';//○
	clientBoard[1][1] = 'O';//○
	clientBoard[1][2] = 'O';//○
	//clientBoard[1][3] = 'X';//● //Server will send these commands
	clientBoard[1][3] = 'O';//○
	//clientBoard[1][4] = '+';//■
	//clientBoard[2][3] = '+';//■
	clientBoard[3][4] = 'O';//○
	clientBoard[4][0] = 'O';//○
	//clientBoard[4][1] = 'X';//●
	clientBoard[4][1] = 'O';//●
	clientBoard[4][4] = 'O';//○
	clientBoard[5][0] = 'O';//○
	//clientBoard[5][1] = 'X';//●
	clientBoard[5][1] = 'O';//●
	clientBoard[5][4] = 'O';//○
	printBoard();
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
	string DEFAULT_METHOD = "GET";
	string DEFAULT_PROTOCOL = "HTTP/1.0";
	string DEFAULT_USERAGENT = "HTMLynx/1.0";

	/*Your retriever takes in an input from the command line and parses the server address and file (web page) that is being requested.*/
	int ticker = 0;
	int server_port;
	std::string server_name, server_path;
	std::cout << "Welcome to HTMLynx, the raw HTML browser!" << std::endl;

	if (argc == 1){ //No arguments: Prompt the user
		server_name = DEFAULT_NAME;
		std::cout << "Enter your desired website. Leave empty for default value 'localhost'" << std::endl;
		if (std::cin.peek() != '\n') {std::cin >> server_name;}
	
		server_path = DEFAULT_PATH;
		std::cout << "Enter your desired path. Leave empty for default value '/'" << std::endl;
		if (std::cin.peek() != '\n') {std::cin >> server_path;}
	
		server_port = DEFAULT_PORT;
		std::cout << "Enter your desired port. Leave empty for default value '80'" << std::endl;
		if (std::cin.peek() != '\n') {std::cin >> server_port;}
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
	
	else if (argc == 5){ //four arguments: Assume user input = server_name server_path server_port method
		server_name = argv[1];
		server_path = argv[2];
		server_port = stoi(argv[3]);
		DEFAULT_METHOD = "POST";
	}

	else{ //None of the above: Exit
		errChk(-1, "Usage: ./client.out [server_name] [server_path] [server_port]");
	}

	initBoard();
	std::cout << "Welcome to Sea Battle, a game by Team Cookies" << std::endl;

	std::cout << "Website: " << server_name << " Path: " << server_path << " Port: " << server_port << std::endl;

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
	std::string swritebuf = DEFAULT_METHOD + " " + server_path + " " + DEFAULT_PROTOCOL + "\r\nUser-Agent: " + DEFAULT_USERAGENT + "\r\n\r\n";
	const char * writebuf = swritebuf.c_str();
	write( clientSd, writebuf, strlen(writebuf) ); // single write: allocates an nbufs-sized array of data buffers, and thereafter calls write( ) to send this array, (i.e., all data buffers) at once.
	
	/* Response format: [protocol] [code] [status]\r\n[header]\r\n[HTML]\r\n\r\n */
	string protocol, code, status;
	char readbuf[2];

	while(read(clientSd, readbuf, 1)){
		if (ticker++ > INT_MAX){errChk(-1, "Trapped in protocol loop!");}
		if (readbuf[0] == ' ' || readbuf[1] == ' ') break;
		protocol += readbuf;
		bzero(readbuf, sizeof(readbuf));
	}

	while(read(clientSd, readbuf, 1)){
		if (ticker++ > INT_MAX){errChk(-1, "Trapped in code loop!");}
		if (readbuf[0] == ' ' || readbuf[1] == ' ') break;
		code += readbuf;
		bzero(readbuf, sizeof(readbuf));
	}

	while(read(clientSd, readbuf, 1)){
		if (ticker++ > INT_MAX){errChk(-1, "Trapped in status loop!");}
		if (readbuf[0] == '\r' || readbuf[0] == '\n' || readbuf[1] == '\r' || readbuf[1] == '\n') break;
		status += readbuf;
		bzero(readbuf, sizeof(readbuf));
	}
	
	cout << protocol << " " << code << " " << status << "\r\n";

	/*When the file is returned by the server, the retriever outputs the file to the screen and saves the retrieved file to the file system.*/
	if (code == "200"){
		ofstream o;
		o.open(DEFAULT_FILE);
		errChk(o.is_open() - 1, "Error: Opening filesystem");
		while(read(clientSd, readbuf, 1)){
			if (ticker++ > INT_MAX){errChk(-1, "Trapped in code==200 loop!");}
			o << readbuf;
			cout << readbuf;
			bzero(readbuf, sizeof(readbuf));
		}
		o.close();
	}
	/*If the server returns an error code instead of an OK code, then the retriever should not save the file and should display on the screen whatever error page was sent with the error.*/
	else{
		cout << protocol << " " << code << " " << status << "\r\n";
		while(read(clientSd, readbuf, 1)){
			if (ticker++ > INT_MAX){errChk(-1, "Trapped in code!=200 loop!");}
			cout << readbuf;
			bzero(readbuf, sizeof(readbuf));
		}
	}

	/*Your retriever should exit after receiving the response.*/
	close(clientSd);
	exit(0);
}