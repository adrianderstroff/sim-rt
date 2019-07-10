#include "console.h"

void rt::console::print(std::string msg) {
	std::cout << msg;
}

void rt::console::println(std::string msg) {
	std::cout << msg << std::endl;
}

void rt::console::progress(std::string msg, double percentage, unsigned int barwidth) {
	//  calculate percentage
	unsigned int bar = barwidth * percentage;
	int percentage100 = static_cast<int>(percentage * 100.0);
	std::string number = (percentage100 < 10) ? "  " : ((percentage100 < 100) ? " " : "");
	number += std::to_string(percentage100) + "%";

	// get spinner position
	std::string spin = spinner();

	// calculate status bar
	std::string progress = " [";
	for (unsigned int i = 0; i < barwidth; ++i) {
		progress += (i < bar) ? "=" : ((i == bar) ? ">" : " ");
	}
	progress += "] " + number + " " + spin;
	progress += (percentage == 1.0) ? "\n" : "\r";

	print(msg + progress);
}

std::string rt::console::spinner() {
	// get current time since epoch in milliseconds
	// scale them by some multiplier to adapt speed
	auto now = std::chrono::system_clock::now();
	auto milliseconds = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto count = milliseconds.time_since_epoch().count() / 200;
	
	// determine position of the spinner
	std::string cursor;
	if      (count % 4 == 0) cursor = "|";
	else if (count % 4 == 1) cursor = "/";
	else if (count % 4 == 2) cursor = "-";
	else if (count % 4 == 3) cursor = "\\";

	return cursor;
}