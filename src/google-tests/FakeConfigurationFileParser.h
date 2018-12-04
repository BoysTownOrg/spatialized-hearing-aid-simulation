#pragma once

#include <dsl-prescription/DslPrescription.h>
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <map>

class FakeConfigurationFileParser : public ConfigurationFileParser {
	std::map<std::string, std::vector<double>> vectors{};
	std::map<std::string, double> doubles{};
	std::map<std::string, int> ints{};
public:
	void setValidSingleChannelDslProperties() {
		using dsl_prescription::Property;
		vectors[propertyName(Property::crossFrequenciesHz)] = {};
		vectors[propertyName(Property::compressionRatios)] = { 0 };
		vectors[propertyName(Property::kneepointGains_dB)] = { 0 };
		vectors[propertyName(Property::kneepoints_dBSpl)] = { 0 };
		vectors[propertyName(Property::broadbandOutputLimitingThresholds_dBSpl)] = { 0 };
	}

	void setValidBrirProperties() {
		using binaural_room_impulse_response::Property;
		vectors[propertyName(Property::leftImpulseResponse)] = { 0 };
		vectors[propertyName(Property::rightImpulseResponse)] = { 0 };
		ints[propertyName(Property::sampleRate)] = 0;
	}

	void setVectorProperty(std::string property, std::vector<double> v) {
		vectors[property] = v;
	}

	void setDoubleProperty(std::string property, double x) {
		doubles[property] = x;
	}

	void setIntProperty(std::string property, int x) {
		ints[property] = x;
	}

	std::vector<double> asVector(std::string property) const override {
		return vectors.at(property);
	}

	double asDouble(std::string property) const override {
		return doubles.at(property);
	}

	int asInt(std::string property) const override {
		return ints.at(property);
	}
};

class MockParserFactory : public ConfigurationFileParserFactory {
	std::shared_ptr<ConfigurationFileParser> parser;
public:
	explicit MockParserFactory(
		std::shared_ptr<ConfigurationFileParser> parser =
		std::make_shared<FakeConfigurationFileParser>()
	) :
		parser{ std::move(parser) } {}

	std::shared_ptr<ConfigurationFileParser> make(std::string filePath) override
	{
		return parser;
	}
};

class ErrorParser : public ConfigurationFileParser {
	std::string message;
public:
	explicit ErrorParser(std::string message) :
		message{ std::move(message) } {}

	double asDouble(std::string) const override {
		throw ParseError{ message };
	}

	int asInt(std::string) const override {
		throw ParseError{ message };
	}

	std::vector<double> asVector(std::string) const override {
		throw ParseError{ message };
	}
};