#include <string.h>
#define TODO 1024

int Etc::errChk(int errVal, std::string errMsg){ // Call with int errVal and string errMsg. If errVal is under 0, cerr errMsg and exit with errVal under 0. Else, return errVal at or over 0.
	if (errVal < 0){
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal;
}

std::string Etc::consoleIn() {
    std::string response;
    std::getline(std::cin, response);
    return response;
}

void Etc::consoleOut(std::string request) {
	std::cout << request; // TODO: stagger char prints by 0.01 sec.
}

void Etc::portConnect( int clientSd, sockaddr_in sendSockAddr ) {
	connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) );
}

int Etc::portAccept( int serverSd, sockaddr_in newSockAddr ) {
    socklen_t newSockAddrSize = sizeof(newSockAddr); // TODO: Redundant storage,
	int newSd = accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize ); // could be put in-line here
    return newSd;
}

std::string Etc::portIn(int newSd) {
	// TODO This code attempts to do a dynamically sized read by reading one character at a time,
	/** // adding it into a string, and terminating when the string no longer lengthens.
	std::string response;
	std::string test = "test";
	char readbuf[2];
	while (true) {
		bzero(readbuf, sizeof(readbuf)); // memset(readbuf, 0, sizeof(readbuf));
		read(newSd, readbuf, 1);
		test = response;
		response += readbuf;
		if (test == response) break;
	}**/

    char response[TODO]; // TODO: sizeof sets to 8 upon compiling. strlen causes futures to hang upon initialization.
    bzero(response, TODO); // memset(readbuf, 0, sizeof(readbuf));
	//recv(newSd, ...)
    read(newSd, response, TODO);
    return std::string(response);
}

void Etc::portOut( int clientSd, std::string request ) {
	//send(clientSd, &request[0], request.size(), 0); //sizeof(response) //strlen(response)
	write(clientSd, request.c_str(), TODO); //single write
}
