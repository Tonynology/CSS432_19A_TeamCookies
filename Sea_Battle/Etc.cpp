int Etc::errChk(int errVal, std::string errMsg){ // Call with int errVal and string errMsg. If errVal is under 0, cerr errMsg and exit with errVal under 0. Else, return errVal at or over 0.
	if (errVal < 0){
		std::cerr << errMsg << std::endl;
		exit(errVal);
	}
	return errVal;
}
