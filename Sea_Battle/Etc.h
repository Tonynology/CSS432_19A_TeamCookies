#ifndef ETC_H
#define ETC_H

class Etc {
	public:
		static int errChk(int errVal, std::string errMsg); // Call with int errVal and string errMsg. If errVal is under 0, cerr errMsg and exit with errVal under 0. Else, return errVal at or over 0.
};

#endif
#include "Etc.cpp"
