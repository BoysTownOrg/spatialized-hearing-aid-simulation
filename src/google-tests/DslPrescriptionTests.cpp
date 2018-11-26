#include <dsl-prescription/DslPrescription.h>
#include <gtest/gtest.h>
#include <map>

class MockDslPrescriptionParser : public DslPrescriptionFileParser {
	std::map<std::string, std::vector<double>> vectors{};
public:
	void setValidSingleChannelProperties() {
		setVectorProperty("cross_frequencies_Hz", {});
		setVectorProperty("compression_ratios", { 0 });
		setVectorProperty("kneepoint_gains_dB", { 0 });
		setVectorProperty("kneepoints_dB_SPL", { 0 });
		setVectorProperty("BOLT_dB_SPL", { 0 });
	}
	void setVectorProperty(std::string property, std::vector<double> v) {
		vectors[property] = v;
	}
	std::vector<double> asVector(std::string property) const override {
		return vectors.at(property);
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