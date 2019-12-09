#ifndef PLAYER_H
#define PLAYER_H

class Player {
	public:
        static int getIPort( );
        static std::string getIAddress( );
        static int getUPort( );
        static std::string getUAddress( );
        static int getServerSd( );
        static int getNewSd( );
        static int getClientSd( );
        
        static void setIPort( int i );
        static void setIAddress( std::string i );
        static void setUPort( int u );
        static void setUAddress( std::string u );
        static void setServerSd( int s );
        static void setNewSd( int n );
        static void setClientSd( int c );
        
        static int main( int argc, char *argv[] );
};

#endif
