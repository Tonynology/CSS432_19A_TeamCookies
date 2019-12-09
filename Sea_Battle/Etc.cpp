#define TODO 1024

int Etc::errChk(int errVal, std::string errMsg){
	if (errVal < 0){
		Etc::consoleErr(errMsg);
		exit(errVal);
	}
	return errVal;
}

void *Etc::dots(void *){
	while (true){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "." << std::flush;
	}
}

void Etc::startDots(pthread_t &t){
	Etc::errChk(pthread_create(&t, NULL, Etc::dots, NULL), "Error: Thread failed to create.");
	Etc::errChk(pthread_detach(t), "Error: Thread failed to detach.");
	Etc::errChk(pthread_join(t, NULL), "Error: Thread failed to join.");
}

void Etc::stopDots(pthread_t &t){
	pthread_cancel(t);
}

std::string Etc::consoleIn() {
    std::string response;
    std::getline(std::cin, response);
    return response;
}

void Etc::consoleOut(std::string request) {
	for (char c : request) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cout << c << std::flush;
	}
}

void Etc::consoleErr(std::string request) {
	for (char c : request) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		std::cerr << c << std::flush;
	}
}

void Etc::serverSetup(int port, int &serverSd){
    sockaddr_in acceptSockAddr;
    bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
    acceptSockAddr.sin_family      = AF_INET;
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port        = htons( port );
    serverSd = Etc::errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Stream-oriented socket failed to open.");
    const int on = 1;
    Etc::errChk(setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, 
                sizeof( int ) ), "Error: SO_REUSEADDR failed to set.");
    Etc::errChk(bind( serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ) ), "Error: Socket failed to bind.");
    Etc::errChk(listen( serverSd, 5 ), "Error: Operating system failed to listen.");
}

void Etc::clientSetup(int port, std::string address, int &clientSd, sockaddr_in &sendSockAddr){
	struct hostent* host = gethostbyname( address.c_str() );
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET;
    sendSockAddr.sin_addr.s_addr =
      inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( port );
	clientSd = Etc::errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Opening stream socket");
}

void Etc::portConnect( int clientSd, sockaddr_in sendSockAddr ) {
	connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) );
}

int Etc::portAccept( int serverSd, sockaddr_in newSockAddr ) {
    socklen_t newSockAddrSize = sizeof(newSockAddr);
	int newSd = accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize );
	return newSd;
}

std::string Etc::portIn(int newSd) {
	/** TODO: sizeof sets to 8 upon compiling. strlen causes futures to hang upon initialization. 
	This code attempts to do a dynamically sized read by reading one character at a time,
	adding it into a string, and terminating when the string no longer lengthens.
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

    char response[TODO];
    bzero(response, TODO); // memset(response, 0, sizeof(response));
	//recv(newSd, &response[0], response.size(), 0); //sizeof (response) //strlen(response)
    read(newSd, response, TODO);
    return std::string(response);
}

void Etc::portOut( int clientSd, std::string request ) {
	//send(clientSd, &request[0], request.size(), 0); //sizeof(request) //strlen(request)
	write(clientSd, request.c_str(), TODO); //single write
}
