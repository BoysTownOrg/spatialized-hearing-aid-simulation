#include "assert-utility.h"
#include "FakeConfigurationFileParser.h"
#include <dsl-prescription/PrescriptionAdapter.h>
#include <gtest/gtest.h>
#include <map>

static void assertInvalidPrescriptionThrownOnChannelCountMismatch(
	std::string property
) {
	try {
		const auto parser = std::make_shared<FakeConfigurationFileParser>();
		parser->setValidSingleChannelDslProperties();
		parser->setVectorProperty(std::move(property), {});
		PrescriptionAdapter adapter{ std::make_shared<FakeConfigurationFileParserFactory>(parser) };
		adapter.read({});
		FAIL() << "Expected PrescriptionAdapter::InvalidPrescription.";
	}
	catch (const PrescriptionAdapter::ReadError &e) {
		assertEqual("channel count mismatch in prescription.", e.what());
	}
}

class PrescriptionAdapterTestCase : public ::testing::TestCase {};

TEST(PrescriptionAdapterTestCase, constructorThrowsInvalidPrescriptionOnChannelCountMismatches) {
	using namespace dsl_prescription;
	for (auto property :
		{
			propertyName(Property::compressionRatios),
			propertyName(Property::kneepointGains_dB),
			propertyName(Property::kneepoints_dBSpl),
			propertyName(Property::broadbandOutputLimitingThresholds_dBSpl)
		}
	)
		assertInvalidPrescriptionThrownOnChannelCountMismatch(property);
}

TEST(
	PrescriptionAdapterTestCase,
	parametersReceivedAsParsed
) {
	using namespace dsl_prescription;
	const auto parser = std::make_shared<FakeConfigurationFileParser>();
	parser->setVectorProperty(propertyName(Property::crossFrequenciesHz), { 3 });
	parser->setVectorProperty(propertyName(Property::compressionRatios), { 4, 4 });
	parser->setVectorProperty(propertyName(Property::kneepointGains_dB), { 5, 5 });
	parser->setVectorProperty(propertyName(Property::kneepoints_dBSpl), { 6, 6 });
	parser->setVectorProperty(
		propertyName(Property::broadbandOutputLimitingThresholds_dBSpl), 
		{ 7, 7 }
	);
	PrescriptionAdapter prescription{ 
		std::make_shared<FakeConfigurationFileParserFactory>(parser) 
	};
	const auto dsl = prescription.read({});
	EXPECT_EQ(2, dsl.channels);
	assertEqual({ 3 }, dsl.crossFrequenciesHz);
	assertEqual({ 4, 4 }, dsl.compressionRatios);
	assertEqual({ 5, 5 }, dsl.kneepointGains_dB);
	assertEqual({ 6, 6 }, dsl.kneepoints_dBSpl);
	assertEqual({ 7, 7 }, dsl.broadbandOutputLimitingThresholds_dBSpl);
}

TEST(
	PrescriptionAdapterTestCase,
	throwsWhenParserThrows)
{
	try {
		const auto parser = std::make_shared<ErrorParser>("error.");
		PrescriptionAdapter adapter{ std::make_shared<FakeConfigurationFileParserFactory>(parser) };
		adapter.read({});
		FAIL() << "Expected DslPrescriptionFileReader::InvalidPrescription.";
	}
	catch (const PrescriptionAdapter::ReadError &e) {
		assertEqual("error.", e.what());
	}
}