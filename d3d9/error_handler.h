#pragma once

#include <fstream>

class ErrorHandler{
public:
	static bool init(){
		myfile = std::ofstream("C:/Users/saumyamukul/Desktop/Output1.txt", std::ios_base::out);
	}
	static void log_error(const std::string& error){
		myfile << error.c_str()<< std::endl;
	}

private:
	static std::ofstream myfile;
};