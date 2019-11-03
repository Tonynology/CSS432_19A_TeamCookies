//#include <iostream>
//#include <cmath>
//#include "board.h"
#define BOARD_SIZE 6
#define SHIP 'O' //○
#define HIT 'X' //●
#define SEA '-' //□
#define MISS '+' //■
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"

using namespace std;

//Board::Board(){
//	initBoardSea();
//}


int Board::errChk(int errVal, string errMsg){
	//errVals under 0 will result in printing errMsg and exiting the program.
	//errVals over 0 will return the errVal.
	if (errVal < 0) {
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal;
}

char Board::getBoard(int x, int y){
	return board[x][y];
}
void Board::setBoard(int x, int y, char c){
	board[x][y] = c;
}

void Board::printBoard(){
	std::cout << "  ";
	for (int k = 1; k <= BOARD_SIZE; k++) {
		cout << std::to_string(k) << " ";
	}
	std::cout << std::endl;
	
	for (int i = 0; i < BOARD_SIZE; i++) {
		std::cout << ALPHABET[i % BOARD_SIZE] << " ";
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::cout << getBoard(i, j) << " ";
		};
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
int Board::to_int(char c){
	return c - 48; //convert from 
}

int Board::atkHlp(char c){
	//TODO: Must be a better way of doing this.
	//Maybe switch statement? Maybe key-value dict map?
	//Also, should support the full alphabet, not just six, since we don't have this hard coded.
	
	//TODO: Split this into a converter board.
	if (c == 'a'){
		return 0;
	}
	else if (c == 'b'){
		return 1;
	}
	else if (c == 'c'){
		return 2;
	}
	else if (c == 'd'){
		return 3;
	}
	else if (c == 'e'){
		return 4;
	}
	else if (c == 'f'){
		return 5;
	}
	else if (c == 'b'){
		return 6;
	}
	else if (c == '1'){
		return 0;
	}
	else if (c == '2'){
		return 1;
	}
	else if (c == '3'){
		return 2;
	}
	else if (c == '4'){
		return 3;
	}
	else if (c == '5'){
		return 4;
	}
	else if (c == '6'){
		return 5;
	}
	else if (c == '7'){
		return 6;
	}
	else{
		errChk(-1, "invalid input: " + to_string(c)); 
	}
}

std::string Board::attackBoard(char a, char b){
	//TODO: Support more formats than just [char][num]
	int x = atkHlp(a);
	int y = atkHlp(b);
	char c = getBoard(x, y);
	
	if (c == SHIP){
		setBoard(x, y, HIT);
		return "hit!\n";
	}
	else if (c == HIT){
		setBoard(x, y, HIT);
		return "hey, you already hit this spot...\n";
	}
	else if (c == SEA){
		setBoard(x, y, MISS);
		return "miss!\n";
	}
	else if (c == MISS){
		setBoard(x, y, MISS);
		return "hey, you already missed this spot...\n";
	}
}

void Board::initBoardSea(){
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			setBoard(i, j, SEA);
		};
	}
}

void Board::initBoardShips(){//TODO: Place ships based on randomly generated list of coordinates?
	//setBoard(0, 3, MISS); //To be entered by user
	setBoard(1, 0, SHIP);
	setBoard(1, 1, SHIP);
	setBoard(1, 2, SHIP);
	setBoard(1, 3, SHIP); //setBoard(1, 3, HIT);
	//setBoard(1, 4, MISS);
	//setBoard(2, 3, MISS);
	setBoard(3, 4, SHIP);
	setBoard(4, 0, SHIP);
	setBoard(4, 1, SHIP); //setBoard(4, 1, HIT);
	setBoard(4, 4, SHIP);
	setBoard(5, 0, SHIP);
	setBoard(5, 1, SHIP); //setBoard(5, 1, HIT);
	setBoard(5, 4, SHIP);
}
