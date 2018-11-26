#pragma once
#include <stdexcept>
#include <string>

#define RUNTIME_ERROR(class_name) \
    class class_name : public std::runtime_error {\
	public:\
		explicit class_name(std::string s) : std::runtime_error{ s } {}\
	};