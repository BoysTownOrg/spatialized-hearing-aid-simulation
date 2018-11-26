#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>
#include <string>

class DslPrescriptionFileParser {
public:
	RUNTIME_ERROR(ParseError);
	INTERFACE_OPERATIONS(DslPrescriptionFileParser);
	virtual std::vector<double> asVector(std::string property) const = 0;
	virtual double asDouble(std::string property) const = 0;
	virtual int asInt(std::string property) const = 0;
};

#include <memory>

class DslPrescriptionFileParserFactory {
public:
	INTERFACE_OPERATIONS(DslPrescriptionFileParserFactory);
	virtual std::shared_ptr<DslPrescriptionFileParser> make(std::string filePath) = 0;
};

