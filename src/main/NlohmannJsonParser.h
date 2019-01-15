#pragma once

#include <dsl-prescription/ConfigurationFileParser.h>
#include <nlohmann/json.hpp>

class NlohmannJsonParser : public ConfigurationFileParser {
	nlohmann::json json{};
public:
	explicit NlohmannJsonParser(std::string filePath);
	std::vector<double> asVector(std::string property) const override;
	double asDouble(std::string property) const override;
	int asInt(std::string property) const override;
private:
	template<typename T>
	T at(std::string property) const {
		try {
			return json.at(std::move(property));
		}
		catch (const nlohmann::json::out_of_range &e) {
			throw ParseError{ e.what() };
		}
	}
};

class NlohmannJsonParserFactory : public ConfigurationFileParserFactory {
	std::shared_ptr<ConfigurationFileParser> make(std::string filePath) override;
};