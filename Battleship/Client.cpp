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
#define SERVER_NAME "localhost"
#define REPETITION 20000
#define NBUFS 6
#define BUFSIZE 6
#define TYPE 3

#include <string>
#include <iostream>
#include <sys/time.h>

bool bad_server_port(int server_port)
{
	if (server_port < 1024 || server_port > 9999)
	{
		std::cout << "valid server_port numbers are between 1024 and 9999" << std::endl;
		return true;
	}
	else return false;
}

bool bad_server_name(const char* server_name)
{
	return false;
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

bool bad_nbufs(int nbufs)
{
	if (nbufs < 0)
	{
		std::cout << "valid nbufs numbers are greater than 0" << std::endl;
		return true;
	}
	else return false;
}

bool bad_bufsize(int bufsize)
{
	if (bufsize < 0)
	{
		std::cout << "valid bufsize numbers are greater than 0" << std::endl;
		return true;
	}
	else return false;
}

bool bad_nbufs_bufsize(int nbufs, int bufsize)
{
	if (nbufs * bufsize != 6 * 6)
	{
		std::cout << "valid nbufs * bufsize == 1500" << std::endl;
		return true;
	}
	else return false;
}

bool bad_type(int type)
{
	if (type < 1 || type > 4)
	{
		std::cout << "valid type numbers are between 1 and 3" << std::endl;
		return true;
	}
	return false;
}

int main(int argc, char const *argv[]) 
{
	int server_port = YOUR_ID;
	if (argc > 1) server_port = atoi(argv[1]);
	const char* server_name = SERVER_NAME;
	if (argc > 2) server_name = argv[2];
	int repetition = REPETITION;
	if (argc > 3) repetition = atoi(argv[3]);
	int nbufs = NBUFS;
	if (argc > 4) nbufs = atoi(argv[4]);
	int bufsize = BUFSIZE;
	if (argc > 5) bufsize = atoi(argv[5]);
	bad_nbufs_bufsize(nbufs, bufsize);
	int type = TYPE;
	if (argc > 6) type = atoi(argv[6]);
	
	if (argc > 7 || bad_server_port(server_port) || bad_server_name(server_name) || bad_repetition(repetition) || bad_nbufs(nbufs) || bad_bufsize(bufsize) || bad_type(type))
	{
		std::cout << "Usage: ./client.out [server_port] [server_name] [repetition] [nbufs] [bufsize] [type]" << std::endl;
	}
	char databuf[nbufs][bufsize];
	
	//Initialize with empty squares
	for (int i = 0; i < nbufs; i++) {
		for (int j = 0; j < bufsize; j++) {
			databuf[i][j] = '□';
		}
	}
	
	//Users fill in squares of their choice
	databuf[0][3] = '■';
	databuf[1][0] = '○';
	databuf[1][1] = '○';
	databuf[1][2] = '○';
	databuf[1][2] = '●';
	databuf[2][3] = '■';
	databuf[3][4] = '○';
	databuf[4][0] = '○';
	databuf[4][1] = '●';
	databuf[4][4] = '○';
	databuf[5][0] = '○';
	databuf[5][1] = '●';
	databuf[5][4] = '○';

	std::cout << server_port << server_name << repetition << nbufs << bufsize << type << std::endl;
	
	struct hostent* host = gethostbyname( server_name );
    sockaddr_in sendSockAddr;
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
    sendSockAddr.sin_family      = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
    sendSockAddr.sin_port        = htons( server_port );
	int clientSd = socket( AF_INET, SOCK_STREAM, 0 );
	connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) );
	struct timeval start, lap, stop;
	gettimeofday(&start, NULL);
	for (int i = 0; i < repetition; i++)
	{
		if (type == 1)
		{
			for ( int j = 0; j < nbufs; j++ )			
			write( clientSd, databuf[j], bufsize );
		}
		else if (type == 2)
		{
			struct iovec vector[nbufs];
			for ( int j = 0; j < nbufs; j++ ) {
			vector[j].iov_base = databuf[j];
			vector[j].iov_len = bufsize;
			}
			writev( clientSd, vector, nbufs );
		}
		else if (type == 3)
		{
			write( clientSd, databuf, nbufs * bufsize );
		}
	}
	gettimeofday(&lap, NULL);
	int count;
	read( clientSd , &count , sizeof(count) ); 
	gettimeofday(&stop, NULL);
	close(clientSd);
}