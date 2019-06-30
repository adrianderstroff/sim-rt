#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <algorithm>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>

namespace rt {

static inline std::string ltrim(std::string s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
	return s;
}

static inline std::string rtrim(std::string s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
	return s;
}

static inline std::string trim(std::string s) {
	return ltrim(rtrim(s));
}

static inline std::vector<std::string> split(const std::string& s, char delimiter) {
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream stream(s);
	while(std::getline(stream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}

}

#endif//STRING_UTIL_H