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
	using namespace dsl_prescription;
	for (const std::string property :
		{
			propertyName(Property::compressionRatios),
			propertyName(Property::kneepointGains_dB),
			propertyName(Property::kneepoints_dBSpl),
			propertyName(Property::broadbandOutputLimitingThresholds_dBSpl)
		})
		assertInvalidPrescriptionThrownOnChannelCountMismatch(property);
}

TEST(
	DslPrescriptionTestCase,
	parametersReceivedAsParsed)
{
	using namespace dsl_prescription;
	MockConfigurationFileParser parser{};
	parser.setVectorProperty(propertyName(Property::crossFrequenciesHz), { 3 });
	parser.setVectorProperty(propertyName(Property::compressionRatios), { 4, 4 });
	parser.setVectorProperty(propertyName(Property::kneepointGains_dB), { 5, 5 });
	parser.setVectorProperty(propertyName(Property::kneepoints_dBSpl), { 6, 6 });
	parser.setVectorProperty(
		propertyName(Property::broadbandOutputLimitingThresholds_dBSpl), 
		{ 7, 7 }
	);
	DslPrescription prescription{ parser };
	EXPECT_EQ(2, prescription.channels());
	assertEqual({ 3 }, prescription.crossFrequenciesHz());
	assertEqual({ 4, 4 }, prescription.compressionRatios());
	assertEqual({ 5, 5 }, prescription.kneepointGains_dB());
	assertEqual({ 6, 6 }, prescription.kneepoints_dBSpl());
	assertEqual({ 7, 7 }, prescription.broadbandOutputLimitingThresholds_dBSpl());
}

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