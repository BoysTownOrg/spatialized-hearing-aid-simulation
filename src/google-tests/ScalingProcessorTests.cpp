#include "assert-utility.h"
#include <signal-processing/ScalingProcessor.h>
#include <gtest/gtest.h>

namespace {
	class ScalingProcessorTests : public ::testing::Test {
	protected:
		using signal_type = ScalingProcessor::signal_type;
		using buffer_type = std::vector<signal_type::element_type>;
	};

	TEST_F(ScalingProcessorTests, processScalesAccordingToScalar) {
		ScalingProcessor processor{ 0.5 };
		buffer_type x{ 1, 2, 3 };
		processor.process(x);
		assertEqual({ 1 * 0.5, 2 * 0.5, 3 * 0.5 }, x);
	}

	TEST_F(ScalingProcessorTests, groupDelayReturnsZero) {
		ScalingProcessor processor{ {} };
		assertEqual(0, processor.groupDelay());
	};
}
