#pragma once

#include <dsl-prescription/PrescriptionAdapter.h>
#include <map>

class FakeConfigurationFileParser : public ConfigurationFileParser {
	std::map<std::string, std::vector<double>> vectors{};
	std::map<std::string, double> doubles{};
	std::map<std::string, int> ints{};
public:
	void setValidSingleChannelDslProperties() {
		using namespace dsl_prescription;
		vectors[propertyName(Property::crossFrequenciesHz)] = {};
		vectors[propertyName(Property::compressionRatios)] = { 0 };
		vectors[propertyName(Property::kneepointGains_dB)] = { 0 };
		vectors[propertyName(Property::kneepoints_dBSpl)] = { 0 };
		vectors[propertyName(Property::broadbandOutputLimitingThresholds_dBSpl)] = { 0 };
	}

	void setVectorProperty(std::string property, std::vector<double> v) {
		vectors[std::move(property)] = std::move(v);
	}

	void setDoubleProperty(std::string property, double x) {
		doubles[std::move(property)] = x;
	}

	void setIntProperty(std::string property, int x) {
		ints[std::move(property)] = x;
	}

	std::vector<double> asVector(std::string property) override {
		return vectors.at(std::move(property));
	}

	double asDouble(std::string property) override {
		return doubles.at(std::move(property));
	}

	int asInt(std::string property) override {
		return ints.at(std::move(property));
	}
};

template<typename T>
class ArgumentCollection {
	std::vector<T> collection{};
public:
	bool contains(T item) {
		return std::find(collection.begin(), collection.end(), item) != collection.end();
	}

	auto push_back(T item) {
		return collection.push_back(std::move(item));
	}
};

class FakeConfigurationFileParserFactory : public ConfigurationFileParserFactory {
	ArgumentCollection<std::string> filePaths_{};
	std::shared_ptr<ConfigurationFileParser> parser;
public:
	explicit FakeConfigurationFileParserFactory(
		std::shared_ptr<ConfigurationFileParser> parser =
			std::make_shared<FakeConfigurationFileParser>()
	) :
		parser{ std::move(parser) } {}

	std::shared_ptr<ConfigurationFileParser> make(std::string s) override {
		filePaths_.push_back(std::move(s));
		return parser;
	}

	void setParser(std::shared_ptr<ConfigurationFileParser> p) {
		parser = std::move(p);
	}

	ArgumentCollection<std::string> filePaths() const {
		return filePaths_;
	}
};

class ErrorParser : public ConfigurationFileParser {
	std::string message;
public:
	explicit ErrorParser(std::string message) :
		message{ std::move(message) } {}

	double asDouble(std::string) override {
		throw ParseError{ message };
	}

	int asInt(std::string) override {
		throw ParseError{ message };
	}

	std::vector<double> asVector(std::string) override {
		throw ParseError{ message };
	}
};