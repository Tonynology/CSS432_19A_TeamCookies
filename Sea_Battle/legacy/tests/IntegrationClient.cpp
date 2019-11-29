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

void printEndl(){
	std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
}

void printWlcm(){
	std::cout << "Welcome to Sea Battle v1.3 beta, a game by Team Cookies\nThis is the client application which sends attack requests to the server and marks a map based off of its reponse\nPlease enter coordinates formatted as [char][num]<Enter> (no spaces, no brackets)\n" << std::endl;
}

int errChk(int errVal, string errMsg){
	//errVals under 0 will result in printing errMsg and exiting the program; at or over 0 will return the errVal
	if (errVal < 0) {
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal;
}

int main(int argc, char const *argv[])
{	
	int server_port;
	std::string server_name;
	if (argc == 1){
		server_name = DEFAULT_NAME;
		server_port = DEFAULT_PORT;
	}
	else if (argc == 2){
		server_name = argv[1];
		server_port = DEFAULT_PORT;
	}
	else if (argc == 3){
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
	
	start: /* Start */
	///2: Displays welcome to User; Initialize the map
	std::cout << "IP: " << server_name << " Port: " << server_port << std::endl;
	printEndl();
	printWlcm();
	int turn = 0;
	Board map;
	map.initBoardSea();
	//initBoardShips(map); //Client does not know ship locations
	map.printBoard();

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
	
	///13: if end go to 15; if loop go to 3
	if (turn > 1024) goto end;
	else goto loop;
	
	end: /* End */
	///15: Close the socket and exit the game.
	std::cout << "Game Over!";
	close(clientSd);
	exit(0);
}
