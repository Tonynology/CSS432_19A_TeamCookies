#include "Lobby.h"
#include "Player.cpp"

#include <iostream>
#include <iterator> 
#include <string>
#include <string.h>
#include <stdio.h>
#include <unordered_map>
#include <vector> 

static int setargs(char *args, char **argv)
{
   int count = 0;
   while (isspace(*args)) ++args;
   while (*args) {
     if (argv) argv[count] = args;
     while (*args && !isspace(*args)) ++args;
     if (argv && *args) *args++ = '\0';
     while (isspace(*args)) ++args;
     count++;
   }
   return count;
}
char **parsedargs(char *args, int *argc)
{
   char **argv = NULL;
   int    argn = 0;
   if (args && *args
    && (args = strdup(args))
    && (argn = setargs(args,NULL))
    && (argv = (char **) malloc((argn+1) * sizeof(char *)))) {
      *argv++ = args;
      argn = setargs(args,argv);
   }
   if (args && !argv) free(args);
   *argc = argn;
   return argv;
}
void freeparsedargs(char **argv)
{
  if (argv) {
    free(argv[-1]);
    free(argv-1);
  } 
}

void Lobby::startGame() {
    std::cout << "Welcome to Sea Battle v1.4 beta, a game by Team Cookies.\n" << std::endl;
    startMenu();
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& v) 
{ 
    os << "["; 
    for (int i = 0; i < v.size(); ++i) { 
        os << v[i]; 
        if (i != v.size() - 1) 
            os << ", "; 
    } 
    os << "]\n"; 
    return os; 
}

void Lobby::startMenu() {
    std::cout << "Please select the following options by typing in the number:" << std::endl;
	std::cout << "[1] Register" << std::endl;
	std::cout << "[2] List Games" << std::endl;
	std::cout << "[3] Create Game" << std::endl;
	std::cout << "[4] Join Game" << std::endl;
	std::cout << "[5] Exit Game" << std::endl;
	std::cout << "[6] Unregister" << std::endl;
    std::cout << "[7] Launch server on localhost port 6932" << std::endl;
    std::cout << "[8] Launch client on localhost port 6932" << std::endl;

    std::cout << std::endl;

    int userResponse = 0;
    std::cin >> userResponse;

    switch(userResponse) {
        case 1:
            registerUser();
            break;
        case 2:
            listGames();
            break;
        case 3:
            createGame();
            break;
        case 4:
            joinGame();
            break;
        case 5:
            exitGame();
            break;
        case 6:
            unregisterUser();
            break;
        case 7: {
            /* Credit to stackOverflow user Remo.D for Parse string into argv/argc
            https://stackoverflow.com/questions/1706551/parse-string-into-argv-argc */

            int i;// TODO: Move this into a separate function
            char **av;
            int ac;
            char *as = NULL;

            //if (argc > 1) as = argv[1];
            as = "./player.out 6932 2396 localhost"; //TODO: Add port and address input from user
            av = parsedargs(as,&ac);
            printf("== %d\n",ac);
            for (i = 0; i < ac; i++)
              printf("[%s]\n",av[i]);

            freeparsedargs(av);
            //exit(0);
            Player::main(ac, av);
            break;
        }
        case 8: {
            int i; //see comments above
            char **av;
            int ac;
            char *as = NULL;

            //if (argc > 1) as = argv[1];
            as = "./player.out 2396 6932 localhost";
            av = parsedargs(as,&ac);
            printf("== %d\n",ac);
            for (i = 0; i < ac; i++)
              printf("[%s]\n",av[i]);

            freeparsedargs(av);
            //exit(0);
            Player::main(ac, av);
            break;
        }
        default:
            std::cout << "Not a valid option. Please reselect." << std::endl;
            break;
    }
}

void Lobby::registerUser() {

    struct player p;

    std::cout << "Please enter a name for this user: " << std::endl;
    std::string username = "";
    std::cin >> p.username;

    std::cout << "Please enter a port number between 2000 - 5000" << std::endl;
    std::string username = "";
    std::cin >> p.port;

    std::cout << "Please type ip address for this computer (e.g. uw1-320-15) " << std::endl;
    std::string username = "";
    std::cin >> p.ipAddress;

    std::unordered_map<std::string, std::vector<person>> userData;

    userData[username, userData.push_back(p)];

}

void Lobby::unregisterUser() {
    // std::cout << "running unregisterUser" << std::endl;
    
    // std::cout << "Please enter a name of the user you want to remove: " << std::endl;
    // std::string username = "";
    // std::cin >> username;
    
    // std::vector<int>::iterator iter;

    // iter = std::find(this->listOfUsers.begin(), this->listOfUsers.end(), username);

    // if (iter != this->listOfUsers.end()) {
    //     std::cout << "User found." << std::endl;
    //     listOfUsers.erase(std::remove(listOfUsers.begin(), listOfUsers.end(), username), listOfUsers.end());
    //     std::cout << "User deleted. Returning to the start menu." << std::endl;
    //     std::cout << std::endl;
        
    //     startMenu();
    // } else {
    //     std::cout << "User not found." << std::endl;
    //     std::cout << "Please select the following option by typing in the number: " << std::endl;
    //     std::cout << "[1] Re-enter username." << std::endl;
    //     std::cout << "[2] Return to menu." << std::endl;
    //     std::cout << "[3] Exit game." << std::endl;

    //     int userResponse = 0;
    //     std::cin >> userResponse;

    //     switch(userResponse) {
    //         case 1:
    //             unregisterUser();
    //         case 2:
    //             startGame();
    //         case 3:
    //             return 0;
    //     }
    // }

    // return 0;
}

void Lobby::listGames() {
    std::cout << "running listGames" << std::endl;
}

void Lobby::createGame() {
    std::cout << "running createGame" << std::endl;
}

void Lobby::joinGame() {
    std::cout << "running joinGame" << std::endl;
}

void Lobby::exitGame() {
    std::cout << "running exitGame" << std::endl;
}

bool Lobby::checkDuplicates() {
    std::vector<std::string>::iterator iter = std::unique(this->listOfUsers.begin(), this->listOfUsers.end());
    bool hasDuplicates = !(iter == this->listOfUsers.end());
    return hasDuplicates;
}