class MonoProcessor {};

#include <memory>

class StereoProcessor {
public:
	StereoProcessor(
		std::shared_ptr<MonoProcessor> left,
		std::shared_ptr<MonoProcessor> right
	) {}
	void process(float *, float *, int ) {

	}
};

#include <gtest/gtest.h>

class MockMonoProcessor : public MonoProcessor {
	float *_signal{};
	int _frameCount{};
public:
	const float *signal() const {
		return _signal;
	}
	int frames() const {
		return _frameCount;
	}
};

class StereoProcessorTestCase : public ::testing::TestCase {};

TEST(StereoProcessorTestCase, tbd) {
	const auto left = std::make_shared<MockMonoProcessor>();
	const auto right = std::make_shared<MockMonoProcessor>();
	StereoProcessor stereo{ left, right };
	float x{};
	float y{};
	stereo.process(&x, &y, 1);
	EXPECT_EQ(&x, left->signal());
	EXPECT_EQ(&y, right->signal());
	EXPECT_EQ(1, left->frames());
	EXPECT_EQ(1, right->frames());
}