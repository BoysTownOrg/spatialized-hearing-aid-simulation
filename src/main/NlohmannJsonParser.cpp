#include "NlohmannJsonParser.h"
#include <fstream>

NlohmannJsonParser::NlohmannJsonParser(std::string filePath) {
	try {
		std::ifstream{ filePath } >> json;
	}
	catch (const nlohmann::json::parse_error &e) {
		throw ParseError{ e.what() };
	}
}

std::vector<double> NlohmannJsonParser::asVector(std::string property) const {
	return at<std::vector<double>>(property);
}

double NlohmannJsonParser::asDouble(std::string property) const {
	return at<double>(property);
}

int NlohmannJsonParser::asInt(std::string property) const {
	return at<int>(property);
}
