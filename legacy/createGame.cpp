#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <unistd.h>       // read, write, close
#include <strings.h>      // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev

#include "Player.cpp"

int main( int argc, char *argv[] ) {
  std::string s = "./player.out 1111 uw1-320-12";

  int ac;
  char **av = Etc::parsedargs(s, &ac);
  Player::main(ac, av);

  Etc::freeparsedargs(av);
}
