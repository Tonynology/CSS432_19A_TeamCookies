#ifndef ETC_H
#define ETC_H

class Etc {
	public:
		static int errChk(int errVal, std::string errMsg); // Call with int errVal and string errMsg. If errVal is under 0, cerr errMsg and exit with errVal under 0. Else, return errVal at or over 0.
		static std::string consoleIn(); // Pause and wait for a line of console input and return it.
		static void consoleOut(std::string request); // Prints out request char by char, waiting 0.01 sec between each char.
		static void portConnect( int clientSd, sockaddr_in sendSockAddr );
		static int portAccept( int serverSd, sockaddr_in newSockAddr );
		static std::string portIn(int newSd);
		static void portOut( int clientSd, std::string request );
};

#endif
#include "Etc.cpp"
