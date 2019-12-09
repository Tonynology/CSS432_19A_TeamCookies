#ifndef ETC_H
#define ETC_H

class Etc {
	public:
		static int errChk(int errVal, std::string errMsg); // Call with int errVal and string errMsg. If errVal is under 0, cerr errMsg and exit with errVal under 0. Else, return errVal at or over 0.
		static void *dots(void *); // Prints out one dot (.) every second
		static void startDots(pthread_t &t); // Start printing dots
		static void stopDots(pthread_t &t); // Stop printing dots
		static std::string consoleIn(); // Pause and wait for a line of console input and return it.
		static void consoleOut(std::string request); // Prints out request char by char, waiting 10msec between each char.
		static void consoleErr(std::string request); // Prints out request char by char, waiting 10msec between each char.
		static void serverSetup(int port, int &serverSd); // Local resource setup for server
		static void clientSetup(int port, std::string address, int &clientSd, sockaddr_in &sendSockAddr); // Local resource setup for client
		static void portConnect( int clientSd, sockaddr_in sendSockAddr ); // Remote connection to clientSd for server
		static int portAccept( int serverSd, sockaddr_in newSockAddr ); // Remote connection to serverSd for client
		static std::string portIn(int newSd); // Pause and wait for a line of port input and return it.
		static void portOut( int clientSd, std::string request ); // Sends request to port.
};

#endif
#include "Etc.cpp"
