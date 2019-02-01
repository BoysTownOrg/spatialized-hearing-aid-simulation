#include "assert-utility.h"
#include "FakeConfigurationFileParser.h"
#include <dsl-prescription/PrescriptionAdapter.h>
#include <gtest/gtest.h>

namespace {
	class PrescriptionAdapterTests : public ::testing::Test {
	protected:
		std::shared_ptr<FakeConfigurationFileParser> parser =
			std::make_shared<FakeConfigurationFileParser>();
		FakeConfigurationFileParserFactory factory{ parser };
		PrescriptionAdapter adapter{ &factory };

		void assertReadThrowsReadFailureOnChannelCountMismatch(
			std::string property
		) {
			parser->setValidSingleChannelDslProperties();
			parser->setVectorProperty(std::move(property), {});
			assertReadThrowsReadFailure("channel count mismatch in prescription.");
		}

		void assertReadThrowsReadFailure(std::string what) {
			try {
				adapter.read({});
				FAIL() << "Expected PrescriptionAdapter::ReadFailure.";
			}
			catch (const PrescriptionAdapter::ReadFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
	};

	TEST_F(PrescriptionAdapterTests, readThrowsReadFailureOnChannelCountMismatches) {
		using namespace dsl_prescription;
		for (auto property :
			{
				propertyName(Property::compressionRatios),
				propertyName(Property::kneepointGains_dB),
				propertyName(Property::kneepoints_dBSpl),
				propertyName(Property::broadbandOutputLimitingThresholds_dBSpl)
			}
		)
			assertReadThrowsReadFailureOnChannelCountMismatch(property);
	}

	TEST_F(
		PrescriptionAdapterTests,
		parametersReceivedAsParsed
	) {
		using namespace dsl_prescription;
		parser->setVectorProperty(propertyName(Property::crossFrequenciesHz), { 3 });
		parser->setVectorProperty(propertyName(Property::compressionRatios), { 4, 4 });
		parser->setVectorProperty(propertyName(Property::kneepointGains_dB), { 5, 5 });
		parser->setVectorProperty(propertyName(Property::kneepoints_dBSpl), { 6, 6 });
		parser->setVectorProperty(
			propertyName(Property::broadbandOutputLimitingThresholds_dBSpl),
			{ 7, 7 }
		);
		const auto dsl = adapter.read({});
		assertEqual(2, dsl.channels);
		assertEqual({ 3 }, dsl.crossFrequenciesHz);
		assertEqual({ 4, 4 }, dsl.compressionRatios);
		assertEqual({ 5, 5 }, dsl.kneepointGains_dB);
		assertEqual({ 6, 6 }, dsl.kneepoints_dBSpl);
		assertEqual({ 7, 7 }, dsl.broadbandOutputLimitingThresholds_dBSpl);
	}

	TEST_F(
		PrescriptionAdapterTests,
		throwsWhenParserThrows
	) {
		factory.setParser(std::make_shared<ErrorParser>("error."));
		assertReadThrowsReadFailure("error.");
	}
}