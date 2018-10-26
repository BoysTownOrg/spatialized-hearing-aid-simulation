#include <common-includes/Interface.h>

class MonoProcessor {
public:
	INTERFACE_OPERATIONS(MonoProcessor);
	virtual void process(float *, int) = 0;
};

#include <memory>

class StereoProcessor {
	std::shared_ptr<MonoProcessor> left;
	std::shared_ptr<MonoProcessor> right;
public:
	StereoProcessor(
		std::shared_ptr<MonoProcessor> left,
		std::shared_ptr<MonoProcessor> right
	) :
		left{ std::move(left) },
		right{ std::move(right) } {}

	void process(float *xLeft, float *xRight, int frameCount) {
		left->process(xLeft, frameCount);
		right->process(xRight, frameCount);
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
	virtual void process(float *x, int count) override {
		_signal = x;
		_frameCount = count;
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