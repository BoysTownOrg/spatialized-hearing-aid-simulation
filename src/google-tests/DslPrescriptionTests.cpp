#include "assert-utility.h"
#include <dsl-prescription/DslPrescription.h>
#include <gtest/gtest.h>
#include <map>

class MockDslPrescriptionParser : public DslPrescriptionFileParser {
	std::map<std::string, std::vector<double>> vectors{};
	std::map<std::string, double> doubles{};
	std::map<std::string, int> ints{};
public:
	void setValidSingleChannelProperties() {
		setVectorProperty(
			DslPrescription::propertyName(DslPrescription::Property::crossFrequenciesHz), {});
		setVectorProperty(
			DslPrescription::propertyName(DslPrescription::Property::compressionRatios), { 0 });
		setVectorProperty(
			DslPrescription::propertyName(DslPrescription::Property::kneepointGains_dB), { 0 });
		setVectorProperty(
			DslPrescription::propertyName(DslPrescription::Property::kneepoints_dBSpl), { 0 });
		setVectorProperty(
			DslPrescription::propertyName(DslPrescription::Property::broadbandOutputLimitingThresholds_dBSpl), { 0 });
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

static void assertInvalidPrescriptionThrownOnChannelCountMismatch(
	std::string property)
{
	try {
		MockDslPrescriptionParser parser{};
		parser.setValidSingleChannelProperties();
		parser.setVectorProperty(property, {});
		DslPrescription{ parser };
		FAIL() << "Expected DslPrescription::InvalidPrescription.";
	}
	catch (const DslPrescription::InvalidPrescription &e) {
		assertEqual("channel count mismatch in prescription.", e.what());
	}
}

class DslPrescriptionTestCase : public ::testing::TestCase {};

TEST(DslPrescriptionTestCase, constructorThrowsInvalidPrescriptionOnChannelCountMismatches) {
	for (const std::string property :
		{
			"compression_ratios",
			"kneepoint_gains_dB",
			"kneepoints_dB_SPL",
			"BOLT_dB_SPL"
		})
		assertInvalidPrescriptionThrownOnChannelCountMismatch(property);
}

TEST(
	DslPrescriptionTestCase,
	parametersReceivedAsParsed)
{
	MockDslPrescriptionParser parser{};
	parser.setVectorProperty(
		DslPrescription::propertyName(DslPrescription::Property::crossFrequenciesHz), { 3 });
	parser.setVectorProperty(
		DslPrescription::propertyName(DslPrescription::Property::compressionRatios), { 4, 4 });
	parser.setVectorProperty(
		DslPrescription::propertyName(DslPrescription::Property::kneepointGains_dB), { 5, 5 });
	parser.setVectorProperty(
		DslPrescription::propertyName(DslPrescription::Property::kneepoints_dBSpl), { 6, 6 });
	parser.setVectorProperty(
		DslPrescription::propertyName(DslPrescription::Property::broadbandOutputLimitingThresholds_dBSpl), { 7, 7 });
	DslPrescription prescription{ parser };
	EXPECT_EQ(2, prescription.channels());
	assertEqual({ 3 }, prescription.crossFrequenciesHz());
	assertEqual({ 4, 4 }, prescription.compressionRatios());
	assertEqual({ 5, 5 }, prescription.kneepointGains_dB());
	assertEqual({ 6, 6 }, prescription.kneepoints_dBSpl());
	assertEqual({ 7, 7 }, prescription.broadbandOutputLimitingThresholds_dBSpl());
}

class ErrorParser : public DslPrescriptionFileParser {
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

TEST(
	DslPrescriptionTestCase,
	throwsWhenParserThrows)
{
	try {
		DslPrescription{ ErrorParser{"error."} };
		FAIL() << "Expected DslPrescriptionFileReader::InvalidPrescription.";
	}
	catch (const DslPrescription::InvalidPrescription &e) {
		assertEqual("error.", e.what());
	}
}