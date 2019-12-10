#include <string.h>

#define TODO 1024

int Etc::errChk(int errVal, std::string errMsg){
	if (errVal < 0){
		Etc::consoleErr(errMsg);
		exit(errVal);
	}
	return errVal;
}

void Etc::validateSelection(std::string &selection, bool registered, std::string cUsername, std::string cAddress, int cPort){
    if (selection.size() != 1 || (selection[0] != '1' && selection[0] != '2' && selection[0] != '3' && selection[0] != '4' && selection[0] != '5' && selection[0] != '6'))
    {
		Etc::consoleOut("invalid selection...\n");

        if (registered) Etc::consoleOut("\nyou are registered as " + cUsername + " on " + cAddress + ":" + std::to_string(cPort) + "\n");
        if (!registered) Etc::consoleOut("[int 1] select from the following options:\n");
        if (registered) Etc::consoleOut("[int 2-6] select from the following options:\n");
	    if (!registered) Etc::consoleOut("[1] register user\n");
	    if (registered) Etc::consoleOut("[2] list games\n");
	    if (registered) Etc::consoleOut("[3] create game\n");
	    if (registered) Etc::consoleOut("[4] join game\n");
	    if (registered) Etc::consoleOut("[5] exit game\n");
	    if (registered) Etc::consoleOut("[6] unregister user\n");
        if (registered) Etc::consoleOut("\n");

        selection = Etc::consoleIn();
        validateSelection(selection, registered, cUsername, cAddress, cPort);
    }
}

void Etc::validateCoord(std::string &coord){
    if (coord.size() != 2 || (coord[0] != 'a' && coord[0] != 'b' && coord[0] != 'c' && coord[0] != 'd' && coord[0] != 'e' && coord[0] != 'f') || (coord[1] != '1' && coord[1] != '2' && coord[1] != '3' && coord[1] != '4' && coord[1] != '5' && coord[1] != '6'))
    {
        Etc::consoleOut("invalid coordinates...\n");
        Etc::consoleOut("[char a-f][int 1-6] launch an attack on coordinate: ");
        coord = Etc::consoleIn();
        validateCoord(coord);
    }
}

/* Credit to stackOverflow user Remo.D for Parse string into argv/argc
https://stackoverflow.com/questions/1706551/parse-string-into-argv-argc */
static int setargs(char *args, char **argv)
{
   int count = 0;

   while (isspace(*args)) ++args;
   while (*args) {
     if (argv) argv[count] = args;
     while (*args && !isspace(*args)) ++args;
     if (argv && *args) *args++ = '\0';
     while (isspace(*args)) ++args;
     count++;
   }
   return count;
}

char **Etc::parsedargs(std::string s, int *argc)
{
   /* Credit to stackOverflow user Tony Delroy for  Getting a `char *` or `const char*` from a `string`
   https://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char*/
   char *args = &s[0];

   char **argv = NULL;
   int    argn = 0;

   if (args && *args
    && (args = strdup(args))
    && (argn = setargs(args,NULL))
    && (argv = (char **) malloc((argn+1) * sizeof(char *)))) {
      *argv++ = args;
      argn = setargs(args,argv);
   }

   if (args && !argv) free(args);

   *argc = argn;
   return argv;
}

void Etc::freeparsedargs(char **argv)
{
  if (argv) {
    free(argv[-1]);
    free(argv-1);
  } 
}

void *dots(void *){
	while (true){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout << "." << std::flush;
	}
}

void Etc::startDots(pthread_t &t){
	Etc::errChk(pthread_create(&t, NULL, dots, NULL), "Error: Thread failed to create.");
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
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		std::cout << c << std::flush;
	}
}

void Etc::consoleErr(std::string request) {
	for (char c : request) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
