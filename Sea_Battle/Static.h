#ifndef Static_H
#define ETC_H

class Static {
	public:
		static int errChk(int errVal, std::string errMsg); // Call with int errVal and string errMsg. If errVal is under 0, cerr errMsg and exit with errVal under 0. Else, return errVal at or over 0.
		static int to_int(std::string s); // stoi wrapper with error checking.
		static void validateSelection(std::string &selection, bool registered, std::string cUsername, std::string cAddress, int cPort); //  Ensure that referenced string selection meets protocol standard.
		static void validateCoord(std::string &coord); // Ensure that referenced string coord meets protocol standard.
		static char **parsedargs(std::string s, int *argc); // Converts string (argument) to argv (return value) and argc (passed by reference)
		static void freeparsedargs(char **argv); // frees parsed args
		static void startDots(pthread_t &t); // Start printing 1 dot(.) every second
		static void stopDots(pthread_t &t); // Stop printing dots
		static std::string consoleIn(); // Pause and wait for a line of console input and return it.
		static void consoleOut(std::string request); // Prints out request char by char, waiting 10msec between each char.
		static void consoleErr(std::string request); // Prints out request char by char, waiting 10msec between each char.
		static void serverSetup(int port, int &serverSd); // Local resource setup for server
		static void clientSetup(int port, std::string address, int &clientSd, sockaddr_in &sendSockAddr); // Local resource setup for client
		static void portConnect( int clientSd, sockaddr_in sendSockAddr ); // Remote connection to clientSd for server
		static int portAccept( int serverSd, sockaddr_in newSockAddr ); // Remote connection to serverSd for client
		static std::string portIn(int sd); // Pause and wait for a line of port input and return it.
		static void portOut( int sd, std::string s ); // Sends request to port.
};

#endif
#include "Static.cpp"
