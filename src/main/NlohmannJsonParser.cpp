#include "NlohmannJsonParser.h"
#include <fstream>

NlohmannJsonParser::NlohmannJsonParser(std::string filePath) {
	try {
		std::ifstream file{ std::move(filePath) };
		file >> json;
	}
	catch (const nlohmann::json::parse_error &e) {
		throw ParseError{ e.what() };
	}
}

std::vector<double> NlohmannJsonParser::asVector(std::string property) {
	return at<std::vector<double>>(std::move(property));
}

double NlohmannJsonParser::asDouble(std::string property) {
	return at<double>(std::move(property));
}

int NlohmannJsonParser::asInt(std::string property) {
	return at<int>(std::move(property));
}

std::shared_ptr<ConfigurationFileParser> NlohmannJsonParserFactory::make(std::string filePath) {
	return std::make_shared<NlohmannJsonParser>(std::move(filePath));
}
