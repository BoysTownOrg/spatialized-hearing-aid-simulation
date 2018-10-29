#include <signal-processing/StereoProcessorAdapter.h>
#include <gtest/gtest.h>

class MockStereoProcessor : public StereoProcessor {
	float *_left{};
	float *_right{};
	int _samples{};
public:
	const float *left() const {
		return _left;
	}
	const float *right() const {
		return _right;
	}
	int samples() const {
		return _samples;
	}
	void process(float * xLeft, float * xRight, int samples) override {
		_left = xLeft;
		_right = xRight;
		_samples = samples;
	}
};

class StereoProcessorAdapterTestCase : public ::testing::TestCase {};

TEST(StereoProcessorAdapterTestCase, tbd) {
	const auto processor = std::make_shared<MockStereoProcessor>();
	StereoProcessorAdapter adapter{ processor };
	float left{};
	float right{};
	float *channels[] = { &left, &right };
	adapter.process(channels, 1);
	EXPECT_EQ(&left, processor->left());
	EXPECT_EQ(&right, processor->right());
	EXPECT_EQ(1, processor->samples());
}
