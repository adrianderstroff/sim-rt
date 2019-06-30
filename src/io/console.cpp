#include "console.h"

void rt::console::print(std::string msg) {
	std::cout << msg;
}

void rt::console::println(std::string msg) {
	std::cout << msg << std::endl;
}

void rt::console::progress(std::string msg, double percentage, unsigned int barwidth) {
	unsigned int bar = barwidth * percentage;
	int percentage100 = static_cast<int>(percentage * 100.0);
	std::string number = (percentage100 < 10) ? "  " : ((percentage100 < 100) ? " " : "");
	number += std::to_string(percentage100) + "%";

	std::string progress = " [";
	for (unsigned int i = 0; i < barwidth; ++i) {
		progress += (i < bar) ? "=" : ((i == bar) ? ">" : " ");
	}
	progress += "] " + number;
	progress += (percentage == 1.0) ? "\n" : "\r";

	print(msg + progress);
}