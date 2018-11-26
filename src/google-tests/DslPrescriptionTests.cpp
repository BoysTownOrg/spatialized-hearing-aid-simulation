class DslPrescriptionFileParser {

};

#include <string>
#include <stdexcept>

class DslPrescription {
public:
	class InvalidPrescription : public std::runtime_error {
	public:
		explicit InvalidPrescription(std::string what) : std::runtime_error{ what } {}
	};
	explicit DslPrescription(const DslPrescriptionFileParser &) {}
};

#include <gtest/gtest.h>

class MockDslPrescriptionParser : public DslPrescriptionFileParser {
public:
	void setValidSingleChannelProperties() {}
	void setVectorProperty(std::string, std::vector<double>) {}
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
		EXPECT_EQ("channel count mismatch in prescription.", e.what());
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