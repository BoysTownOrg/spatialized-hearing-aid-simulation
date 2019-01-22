#pragma once

#include <common-includes/Interface.h>
#include <string>

class Documenter {
public:
	INTERFACE_OPERATIONS(Documenter);
	virtual void initialize(std::string filePath) = 0;
	virtual void writeLine(std::string) = 0;
};