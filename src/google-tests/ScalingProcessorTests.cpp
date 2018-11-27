#include <signal-processing/SignalProcessor.h>

class ScalingProcessor : public SignalProcessor {
	double scale;
public:
	explicit ScalingProcessor(double scale) : scale{ scale } {}
	void process(float * signal, int samples) override
	{
		signal;
		samples;
	}
};

#include "assert-utility.h"
#include <gtest/gtest.h>

class ScalingProcessorTestCase : public ::testing::TestCase {};

TEST(ScalingProcessorTestCase, tbd) {
	ScalingProcessor processor{ 0.5 };
	std::vector<float> x{ 1, 2, 3 };
	processor.process(&x[0], 3);
	assertEqual({ 1 * 0.5, 2 * 0.5, 3 * 0.5 }, x);
}