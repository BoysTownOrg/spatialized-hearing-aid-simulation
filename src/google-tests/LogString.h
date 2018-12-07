#pragma once

#include <string>

class LogString {
	std::string s;
public:
	explicit LogString(std::string s = {}) : s{ std::move(s) } {}

	bool isEmpty() const {
		return s.empty();
	}

	bool beginsWith(std::string const &beginning) {
		if (s.length() >= beginning.length())
			return 0 == s.compare(0, beginning.length(), beginning);
		else
			return false;
	}

	bool endsWith(std::string const &ending) {
		if (s.length() >= ending.length())
			return 0 == s.compare(
				s.length() - ending.length(),
				ending.length(),
				ending);
		else
			return false;
	}

	bool contains(std::string const &s2) {
		return s.find(s2) != std::string::npos;
	}

	LogString &operator+=(const LogString &appended) {
		s += appended.s;
		return *this;
	}
};