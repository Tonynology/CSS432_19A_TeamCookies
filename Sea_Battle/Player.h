#ifndef PLAYER_H
#define PLAYER_H

class Player {
	public:
        static int getIPort( );
        static std::string getIAddress( );
        static int getUPort( );
        static std::string getUAddress( );

        static void setIPort( int iPort );
        static void setIAddress( std::string iAddress );
        static void setUPort( int uPort );
        static void setUAddress( std::string uAddress );

        static void playerConnect( int clientSd, sockaddr_in sendSockAddr ); // attempt connection at socket clientSd
        static int playerAccept( int serverSd, sockaddr_in newSockAddr ); // listen for connection at socket serverSd
        static std::string playerCin( ); // listen to standard input stream
        static std::string playerRead( int newSd ); // listen to input at socket newSd
        static void playerWrite( int clientSd, std::string requestString ); // write request to socket clientSd
};

#endif
