#include <common-includes/Interface.h>
#include <stdexcept>
#include <vector>
#include <string>

class DslPrescriptionFileParser {
public:
	INTERFACE_OPERATIONS(DslPrescriptionFileParser);
	class ParseError : public std::runtime_error {
	public:
		explicit ParseError(std::string what) : std::runtime_error{ what } {}
	};
	virtual std::vector<double> asVector(std::string property) const = 0;
};

#include <string>
#include <stdexcept>

class DslPrescription {
	// Order important for construction.
	const std::vector<double> _crossFrequenciesHz;
	const std::vector<double> _compressionRatios;
	const std::vector<double> _kneepointGains_dB;
	const std::vector<double> _kneepoints_dBSpl;
	const std::vector<double> _broadbandOutputLimitingThresholds_dBSpl;
public:
	class InvalidPrescription : public std::runtime_error {
	public:
		explicit InvalidPrescription(std::string what) : std::runtime_error{ what } {}
	};
	explicit DslPrescription(const DslPrescriptionFileParser &parser) :
		_compressionRatios(parser.asVector("compression_ratios")),
		_kneepointGains_dB(parser.asVector("kneepoint_gains_dB")),
		_kneepoints_dBSpl(parser.asVector("kneepoints_dB_SPL")),
		_broadbandOutputLimitingThresholds_dBSpl(
			parser.asVector("BOLT_dB_SPL"))
	{
		const auto channels = _crossFrequenciesHz.size() + 1;
		if (
			_compressionRatios.size() != channels ||
			_kneepointGains_dB.size() != channels ||
			_kneepoints_dBSpl.size() != channels ||
			_broadbandOutputLimitingThresholds_dBSpl.size() != channels
		)
			throw InvalidPrescription{ "channel count mismatch in prescription." };
	}
};

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