#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>

class DslPrescriptionFileParser {
public:
	RUNTIME_ERROR(ParseError);
	INTERFACE_OPERATIONS(DslPrescriptionFileParser);
	virtual std::vector<double> asVector(std::string property) const = 0;
	virtual double asDouble(std::string property) const = 0;
	virtual int asInt(std::string property) const = 0;
};

