#include <string.h>
#include <limits.h>
#define PORT_NUMBER_MAX 65535
#define PORT_NUMBER_MIN 1023

#define DATABUF_MAX 1024

int buggy = 0;

void Static::debug(){
  buggy++;
  Static::consoleErr(std::to_string(buggy));
}

int Static::errChk(int errVal, std::string errMsg){
	if (errVal < 0){
		Static::consoleErr(errMsg);
		exit(errVal);
	}
	return errVal;
}

int Static::to_int(std::string s){
  try{
    return std::stoi(s);
  }
  catch (const std::exception& ia){
    return -1;
  }
}

void Static::validatePort(int &port){
    if (port < PORT_NUMBER_MIN || port > PORT_NUMBER_MAX){
        Static::consoleErr("port must be a number between " + std::string(std::to_string(PORT_NUMBER_MIN)) + " and " + std::string(std::to_string(PORT_NUMBER_MAX)) + ". try again: ");
        port = Static::to_int(Static::consoleIn());
        validatePort(port);
    }
}

//void Static::validateSelection(int &selection){
//  if (selection < 1 || selection > 6){
//    Static::consoleErr("selection must be a number between " + std::string(std::to_string(1)) + " and " + std::string(std::to_string(6)) + ". try again: ");
//    selection = Static::to_int(Static::consoleIn());
//    validateSelection(selection);
//  }
//}

void Static::validateCoord(std::string &coord){
    if (coord.size() != 2 || (coord[0] != 'a' && coord[0] != 'b' && coord[0] != 'c' && coord[0] != 'd' && coord[0] != 'e' && coord[0] != 'f') || (coord[1] != '1' && coord[1] != '2' && coord[1] != '3' && coord[1] != '4' && coord[1] != '5' && coord[1] != '6'))
    {
        Static::consoleOut("invalid coordinates...\n");
        Static::consoleOut("[char a-f][int 1-6] launch an attack on coordinate: ");
        coord = Static::consoleIn();
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

char **Static::parsedargs(std::string s, int *argc)
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

void Static::freeparsedargs(char **argv)
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

void Static::startDots(pthread_t &t){
	Static::errChk(pthread_create(&t, NULL, dots, NULL), "Error: Thread failed to create.");
	Static::errChk(pthread_detach(t), "Error: Thread failed to detach.");
	Static::errChk(pthread_join(t, NULL), "Error: Thread failed to join.");
}

void Static::stopDots(pthread_t &t){
	pthread_cancel(t);
}

std::string Static::consoleIn() {
    std::string response;
    std::getline(std::cin, response);
    return response;
}

void Static::consoleOut(std::string request) {
	for (char c : request) {
    if (c == '\n') std::this_thread::sleep_for(std::chrono::milliseconds(64));
		else std::this_thread::sleep_for(std::chrono::milliseconds(8));
		std::cout << c << std::flush;
	}
}

void Static::consoleErr(std::string request) {
	for (char c : request) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		std::cerr << c << std::flush;
	}
}

void Static::serverSetup(int port, int &serverSd){
    sockaddr_in acceptSockAddr;
    bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
    acceptSockAddr.sin_family      = AF_INET;
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port        = htons( port );
    serverSd = Static::errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Stream-oriented socket failed to open.");
    const int on = 1;
    Static::errChk(setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, 
                sizeof( int ) ), "Error: SO_REUSEADDR failed to set.");
    Static::errChk(bind( serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ) ), "Error: Socket failed to bind.");
    Static::errChk(listen( serverSd, 5 ), "Error: Operating system failed to listen.");
}

void Static::clientSetup(int port, std::string address, int &clientSd, sockaddr_in &sendSockAddr){
	struct hostent* host = gethostbyname( address.c_str() );
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET;
    sendSockAddr.sin_addr.s_addr =
      inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( port );
	clientSd = Static::errChk(socket( AF_INET, SOCK_STREAM, 0 ), "Error: Opening stream socket");
}

void Static::portConnect( int clientSd, sockaddr_in sendSockAddr ) {
	connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) );
}

int Static::portAccept( int serverSd, sockaddr_in newSockAddr ) {
    socklen_t newSockAddrSize = sizeof(newSockAddr);
	int newSd = accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize );
	return newSd;
}

std::string Static::portIn(int sd){
  	/** Test replacement for the read read function.
    TODO: sizeof sets to 8 upon compiling. strlen causes futures to hang upon initialization. 
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

  char c[DATABUF_MAX];
  bzero(c, DATABUF_MAX); // memset(c, 0, sizeof(c));
	//recv(newSd, &c[0], c.size(), 0); //sizeof (c) //strlen(c)
  read(sd, c, DATABUF_MAX);
  write(sd, c, DATABUF_MAX); //single write
  char ac[DATABUF_MAX];
  bzero(ac, DATABUF_MAX); // memset(c, 0, sizeof(c));
	//recv(newSd, &c[0], c.size(), 0); //sizeof (c) //strlen(c)
  read(sd, ac, DATABUF_MAX);
  std::string s = std::string(c);
  std::string as = std::string(ac);
  Static::errChk(-1 + (s == as), "port in acknowledgement failed! " + s + " DNE " + as);
  return as;
}

//std::string Static::portIn(int newSd) {
//
//    char response[DATABUF_MAX];
//    bzero(response, DATABUF_MAX); // memset(response, 0, sizeof(response));
//	//recv(newSd, &response[0], response.size(), 0); //sizeof (response) //strlen(response)
//    read(newSd, response, DATABUF_MAX);
//    return std::string(response);
//}

void Static::portOut( int sd, std::string s) {
  //send(sd, &s[0], s.size(), 0); //sizeof(s) //strlen(s)
	write(sd, s.c_str(), DATABUF_MAX); //single write
  char ac[DATABUF_MAX];
  bzero(ac, DATABUF_MAX); // memset(c, 0, sizeof(c));
	//recv(sd, &c[0], c.size(), 0); //sizeof (c) //strlen(c)
  read(sd, ac, DATABUF_MAX);
  write(sd, ac, DATABUF_MAX);
  std::string as = std::string(ac);
  Static::errChk(-1 + (s == as), "port out acknowledgement failed! " + s + " DNE " + as);
}

//void Static::portOut( int clientSd, std::string request ) {
//	//send(clientSd, &request[0], request.size(), 0); //sizeof(request) //strlen(request)
//	write(clientSd, request.c_str(), DATABUF_MAX); //single write
//}
