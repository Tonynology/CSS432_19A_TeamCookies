#include <iostream>
#include "Board.h"

using namespace std;

int main() {
	Board serverBoard;
	serverBoard.initBoardSea();
	serverBoard.initBoardShips();
	serverBoard.printBoard();
	serverBoard.attackBoard('a', '1');
	serverBoard.printBoard();
	
	Board clientBoard;
	clientBoard.initBoardSea();
	clientBoard.printBoard();
	clientBoard.attackBoard('b', '2');
	clientBoard.printBoard();
}