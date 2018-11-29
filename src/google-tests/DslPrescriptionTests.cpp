#include "assert-utility.h"
#include "MockConfigurationFileParser.h"
#include <dsl-prescription/DslPrescription.h>
#include <gtest/gtest.h>
#include <map>

static void assertInvalidPrescriptionThrownOnChannelCountMismatch(
	std::string property)
{
	try {
		MockConfigurationFileParser parser{};
		parser.setValidSingleChannelDslProperties();
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
	MockConfigurationFileParser parser{};
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