#include <sys/types.h>    
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>    
#include <netdb.h>        
#include <unistd.h>       
#include <strings.h>      
#include <netinet/tcp.h>  
#include <sys/uio.h>      
#define YOUR_ID 6932      
#define REPETITION 20000
#define N 3
#define BUFSIZE 1500

#include <string>
#include <sys/time.h>
#include <iostream>

bool bad_port(int port)
{
	if (port < 1024 || port > 9999)
	{
		std::cout << "valid port numbers are between 1024 and 9999" << std::endl;
		return true;
	}
	else return false;
}

bool bad_repetition(int repetition)
{
	if (repetition < 0)
	{
		std::cout << "valid repetition numbers are greater than 0" << std::endl;
		return true;
	}
	else return false;
}

bool bad_n(int n)
{
	if (n < 0)
	{
		std::cout << "valid concurrent connections are greater than 0" << std::endl;
		return true;
	}
	else return false;
}

int main(int argc, char const *argv[])
{	
	int port = YOUR_ID;
	if (argc > 1) port = atoi(argv[1]);
	int repetition = REPETITION;
	if (argc > 2) repetition = atoi(argv[2]);
	int n = N;
	if (argc > 3) n = atoi(argv[3]);
	if (argc > 4 || bad_port(port) || bad_repetition(repetition) || bad_n(n))
	std::cout << "Usage: ./server.out [port] [repetition] [n]" << std::endl;
    sockaddr_in acceptSockAddr;
    bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
    acceptSockAddr.sin_family      = AF_INET;
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port        = htons( port );
    int serverSd = socket( AF_INET, SOCK_STREAM, 0 );
    const int on = 1;
    setsockopt( serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof( int ) );
    bind( serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ) );
    listen( serverSd, n );
	sockaddr_in newSockAddr;
	socklen_t newSockAddrSize = sizeof( newSockAddr );
	accept:
	int newSd = accept( serverSd, ( sockaddr *)&newSockAddr, &newSockAddrSize );
	char databuf[BUFSIZE]; 
	struct timeval start, stop;
	gettimeofday(&start, NULL);
	int count = 0;
	for (int i = 0; i < repetition; i++)
	{
		for ( int nRead = 0; 
			( nRead += read( newSd, databuf, BUFSIZE - nRead ) ) < BUFSIZE; 
			++count );
	}
	gettimeofday(&stop, NULL);
	write(newSd , &count, sizeof(count)); 
	std::cout << "data-receiving time = " << stop.tv_sec - start.tv_sec << " sec, " << stop.tv_usec - start.tv_usec << " usec" << std::endl;
	close(newSd);
	goto accept;
} 
