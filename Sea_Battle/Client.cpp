/**
CSS 432 AU19
Sea Battle - Intro to Network Programming
Team Cookies

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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
using namespace std; //Duplicated comments will be listed in Server.cpp.

int errChk(int errVal, string errMsg){
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

		//server_name = DEFAULT_NAME;
		//std::cout << "Enter your desired website. Leave empty for default value 'localhost'" << std::endl;
		//if (std::cin.peek() != '\n') {std::cin >> server_name;}
		//
		//server_path = DEFAULT_PATH;
		//std::cout << "Enter your desired path. Leave empty for default value '/'" << std::endl;
		//if (std::cin.peek() != '\n') {std::cin >> server_path;}
		//
		//server_port = DEFAULT_PORT;
		//std::cout << "Enter your desired port. Leave empty for default value '80'" << std::endl;
		//if (std::cin.peek() != '\n') {std::cin >> server_port;}
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
	std::cout << "IP: " << server_name << " Port: " << server_port << std::endl;
	std::cout << "Welcome to Sea Battle v1.0 alpha, a game by Team Cookies" << std::endl;
	std::cout << "This is the client application. Please enter coordinates with using [char][num] (no spaces, no brackets) to attack the ships of the server." << std::endl;
	std::cout << "input 'a1' to get started." << std::endl;

	send:
	std::string swritebuf;
	std::cin >> swritebuf;
	const char * writebuf = swritebuf.c_str();
	write( clientSd, writebuf, strlen(writebuf) ); // single write: allocates an nbufs-sized array of data buffers, and thereafter calls write( ) to send this array, (i.e., all data buffers) at once.
	std::cout << "writing: " << swritebuf << std::endl;
	
	char readbuf[1024];
	bzero(readbuf, sizeof(readbuf));
	read(clientSd, readbuf, sizeof(readbuf));
	std::cout << "reading: " << readbuf << std::endl;

	/*Your retriever should exit after receiving the response.*/
	goto send;
	close(clientSd);
	exit(0);
}