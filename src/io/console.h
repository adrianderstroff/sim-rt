#ifndef CONSOLE_H
#define CONSOLE_H

#include <chrono>
#include <iostream>
#include <string>

namespace rt::console {
	void print(std::string msg);
	void println(std::string msg);
	void progress(std::string msg, double percentage, unsigned int barwidth = 50);
	std::string spinner();
}

#endif//CONSOLE_H