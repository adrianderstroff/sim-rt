#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <algorithm>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>

namespace rt {
	/**
	 * removes all whitespaces from the left of a string
	 */
	inline std::string ltrim(std::string s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
		return s;
	}

	/**
	 * removes all whitespaces from the right of a string
	 */
	inline std::string rtrim(std::string s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
		return s;
	}

	/**
	 * removes all whitespaes from both sides of a string
	 */
	inline std::string trim(std::string s) {
		return ltrim(rtrim(s));
	}

	/**
	 * spits a string into a vector of strings based on a delimiter
	 */
	inline std::vector<std::string> split(const std::string& s, char delimiter) {
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream stream(s);
		while(std::getline(stream, token, delimiter)) {
			tokens.push_back(token);
		}
		return tokens;
	}

	/**
	 * fills up a string with padding to make it a specified length
	 */
	inline std::string right_align(const std::string& msg, size_t length, char pad) {
		// text is longer than the specified length
		if (msg.length() >= length) return msg;

		// calculate how many characters you have to pad
		size_t diff = length - msg.length();

		// create left padding
		std::string padding = "";
		for (size_t i = 0; i < diff; i++) padding += pad;

		// add padding to the string
		return padding + msg;
	}

	/**
	 * fills up a string with padding to make it a specified length
	 */
	inline std::string left_align(const std::string& msg, size_t length, char pad) {
		// text is longer than the specified length
		if (msg.length() >= length) return msg;

		// calculate how many characters you have to pad
		size_t diff = length - msg.length();

		// create right padding
		std::string padding = "";
		for (size_t i = 0; i < diff; i++) padding += pad;

		// add padding to the string
		return msg + padding;
	}

	inline std::string format_time(double time) {
		int h = static_cast<int>(std::floor(time / 3600));
		time -= h * 3600.0;
		int m = static_cast<int>(std::floor(time / 60));
		time -= m * 60.0;
		int s = static_cast<int>(std::floor(time));
		int ms = static_cast<int>(1000*(time - s));

		std::string hh  = right_align(std::to_string(h),  2, '0');
		std::string mm  = right_align(std::to_string(m),  2, '0');
		std::string ss  = right_align(std::to_string(s),  2, '0');
		std::string mms = right_align(std::to_string(ms), 3, '0');

		return hh + ':' + mm + ':' + ss + '.' + mms;
	}
}

#endif//STRING_UTIL_H