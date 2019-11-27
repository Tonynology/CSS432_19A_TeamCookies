#ifndef BOARD_H
#define BOARD_H
#define BOARD_HEIGHT 6
#define BOARD_WIDTH 6

class Board {
	public:
		//Board();
		//~Board();
		int errChk(int errVal, std::string errMsg); //TODO: Make errChk module
		char getBoard(int x, int y);
		void setBoard(int x, int y, char c);
		void printBoard();
		int requestTranslator(char c);
		char responseTranslator(char c);
		std::string attackBoard(int x, int y);
		void initBoardSea();
		void initBoardShips();
		
	private:
		//int height; //TODO: Allow user to set board size. May require refactoring board from an array to vector.
		//int width;
		char board[BOARD_HEIGHT][BOARD_WIDTH];

};

#endif
#include "Board.cpp"
