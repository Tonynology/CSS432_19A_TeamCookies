#ifndef BOARD_H
#define BOARD_H
#define BOARD_HEIGHT 6 //TODO:change size to height and width
#define BOARD_WIDTH 6
#define BOARD_SIZE 6


using namespace std;

class Board {
	public:
		//Board();
		//~Board();
		int errChk(int errVal, string errMsg); //TODO: Make an errChk module
		char getBoard(int x, int y);
		void setBoard(int x, int y, char c);
		void printBoard();
		int to_int(char c);
		int atkHlp(char c);
		std::string attackBoard(char a, char b);
		void initBoardSea();
		void initBoardShips();
		//setBoard(int x, int y);
		//printBoard();
		
	private:
		//int height; //TODO: Allow user to set board size
		//int width;
		char board[BOARD_SIZE][BOARD_SIZE];

};

#endif
#include "Board.cpp"