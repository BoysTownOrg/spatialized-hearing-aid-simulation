#pragma once

#include <common-includes/Interface.h>
#include <stdexcept>
#include <vector>
#include <string>

class DslPrescriptionFileParser {
public:
	INTERFACE_OPERATIONS(DslPrescriptionFileParser);
	class ParseError : public std::runtime_error {
	public:
		explicit ParseError(std::string what) : std::runtime_error{ what } {}
	};
	virtual std::vector<double> asVector(std::string property) const = 0;
	virtual double asDouble(std::string property) const = 0;
	virtual int asInt(std::string property) const = 0;
};

