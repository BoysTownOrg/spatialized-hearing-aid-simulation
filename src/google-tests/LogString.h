#pragma once

#include <sstream>

class LogString {
	std::stringstream s;
public:
	LogString(std::string s = {}) : s{ std::move(s) } {}

	void insert(std::string s_) {
		s << std::move(s_);
	}

	bool isEmpty() const {
		return s.str().empty();
	}

	bool beginsWith(std::string const &beginning) const {
		if (s.str().length() >= beginning.length())
			return 0 == s.str().compare(0, beginning.length(), beginning);
		else
			return false;
	}

	bool endsWith(std::string const &ending) const {
		if (s.str().length() >= ending.length())
			return 0 == s.str().compare(
				s.str().length() - ending.length(),
				ending.length(),
				ending);
		else
			return false;
	}

	bool contains(std::string s2) const {
		return s.str().find(std::move(s2)) != std::string::npos;
	}
	
	operator std::string() const { return s.str(); }
};