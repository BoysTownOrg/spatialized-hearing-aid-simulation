#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>
#include <string>
#include <memory>

class ConfigurationFileParser {
public:
    INTERFACE_OPERATIONS(ConfigurationFileParser)
	virtual std::vector<double> asVector(std::string property) = 0;
	virtual double asDouble(std::string property) = 0;
	virtual int asInt(std::string property) = 0;
    RUNTIME_ERROR(ParseError)
};

class ConfigurationFileParserFactory {
public:
    INTERFACE_OPERATIONS(ConfigurationFileParserFactory)
	virtual std::shared_ptr<ConfigurationFileParser> make(std::string filePath) = 0;
};

