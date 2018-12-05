#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>
#include <string>
#include <memory>

class ConfigurationFileParser {
public:
	RUNTIME_ERROR(ParseError);
	INTERFACE_OPERATIONS(ConfigurationFileParser);
	virtual std::vector<double> asVector(std::string property) const = 0;
	virtual double asDouble(std::string property) const = 0;
	virtual int asInt(std::string property) const = 0;
};

class ConfigurationFileParserFactory {
public:
	INTERFACE_OPERATIONS(ConfigurationFileParserFactory);
	virtual std::shared_ptr<ConfigurationFileParser> make(std::string filePath) = 0;
};

