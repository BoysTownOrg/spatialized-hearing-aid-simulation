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
		setIntProperty("chunk_size", 0);
		setIntProperty("window_size", 0);
		setDoubleProperty("attack_ms", 0);
		setDoubleProperty("release_ms", 0);
		setVectorProperty("cross_frequencies_Hz", {});
		setVectorProperty("compression_ratios", { 0 });
		setVectorProperty("kneepoint_gains_dB", { 0 });
		setVectorProperty("kneepoints_dB_SPL", { 0 });
		setVectorProperty("BOLT_dB_SPL", { 0 });
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
		EXPECT_EQ(std::string{ "channel count mismatch in prescription." }, e.what());
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
	parser.setDoubleProperty("attack_ms", 1);
	parser.setDoubleProperty("release_ms", 2);
	parser.setVectorProperty("cross_frequencies_Hz", { 3 });
	parser.setVectorProperty("compression_ratios", { 4, 4 });
	parser.setVectorProperty("kneepoint_gains_dB", { 5, 5 });
	parser.setVectorProperty("kneepoints_dB_SPL", { 6, 6 });
	parser.setVectorProperty("BOLT_dB_SPL", { 7, 7 });
	parser.setIntProperty("chunk_size", 8);
	parser.setIntProperty("window_size", 9);
	DslPrescription prescription{ parser };
	EXPECT_EQ(2, prescription.channels());
	EXPECT_EQ(1, prescription.attack_ms());
	EXPECT_EQ(2, prescription.release_ms());
	assertEqual({ 3 }, prescription.crossFrequenciesHz());
	assertEqual({ 4, 4 }, prescription.compressionRatios());
	assertEqual({ 5, 5 }, prescription.kneepointGains_dB());
	assertEqual({ 6, 6 }, prescription.kneepoints_dBSpl());
	assertEqual({ 7, 7 }, prescription.broadbandOutputLimitingThresholds_dBSpl());
	EXPECT_EQ(8, prescription.chunkSize());
	EXPECT_EQ(9, prescription.windowSize());
}