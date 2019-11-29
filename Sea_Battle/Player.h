#ifndef PLAYER_H
#define PLAYER_H

class Player {
	public:
        //static int server(int argc, char const *argv[]);
        static void playerConnect( int clientSd, sockaddr_in sendSockAddr ); // attempt connection at socket clientSd
        static int playerAccept( int serverSd, sockaddr_in newSockAddr ); // listen for connection at socket serverSd
        static std::string playerCin(); // listen to standard input stream
        static std::string playerRead(int newSd); // listen to input at socket newSd
        static void playerWrite( int clientSd, std::string requestString ); // write request to socket clientSd
        static int main( int argc, char *argv[] );
};

#endif
