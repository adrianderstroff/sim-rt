#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>
#include <iostream>

namespace rt::console {
	void print(std::string msg);
	void println(std::string msg);
	void progress(std::string msg, double percentage, unsigned int barwidth = 50);
}

#endif//CONSOLE_H