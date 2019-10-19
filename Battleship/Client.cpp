#include <iostream>

using namespace std;

int main( ) {
  
  
  cout << "enter server_port: ";
  cin >> server_port;
  cout << "enter server_name: ";
  cin >> server_name;
  
  struct hostent* host = gethostbyname( server_name );

  int port = YOUR_ID;  // the last 4 digits of your student id
  sockaddr_in sendSockAddr;
  bzero( (char*)&sendSockAddr, sizeof( sendSockAddr ) );
  sendSockAddr.sin_family      = AF_INET; // Address Family Internet
  sendSockAddr.sin_addr.s_addr =
  inet_addr( inet_ntoa( *(struct in_addr*)*host->h_addr_list ) );
  sendSockAddr.sin_port        = htons( server_port );
  
  int clientSd = socket( AF_INET, SOCK_STREAM, 0 );
  connect( clientSd, ( sockaddr* )&sendSockAddr, sizeof( sendSockAddr ) );
}