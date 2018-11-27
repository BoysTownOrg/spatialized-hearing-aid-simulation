#include <audio-stream-processing/AudioFrameProcessor.h>
#include <signal-processing/SignalProcessor.h>
#include <vector>
#include <memory>

class ChannelProcessingGroup : public AudioFrameProcessor {
	std::vector<std::shared_ptr<SignalProcessor>> processors;
public:
	explicit ChannelProcessingGroup(
		std::vector<std::shared_ptr<SignalProcessor>> processors
	) :
		processors{ std::move(processors) } {}

	void process(float **, int) override {
	}
};

#include <gtest/gtest.h>

class MockSignalProcessor : public SignalProcessor {
	float *_signal{};
public:
	void process(float * signal, int) override {
		_signal = signal;
	}
	float *signal() const {
		return _signal;
	}
};

class ChannelProcessorTestCase : public ::testing::TestCase {};

TEST(ChannelProcessorTestCase, tbd) {
	std::vector<std::shared_ptr<MockSignalProcessor>> processors;
	processors.push_back(std::make_shared<MockSignalProcessor>());
	processors.push_back(std::make_shared<MockSignalProcessor>());
	processors.push_back(std::make_shared<MockSignalProcessor>());
	ChannelProcessingGroup group{ { processors[0], processors[1], processors[2] } };
	float a{};
	float b{};
	float c{};
	float *channels[] = { &a, &b, &c };
	group.process(channels, 0);
	EXPECT_EQ(&a, processors[0]->signal());
	EXPECT_EQ(&b, processors[1]->signal());
	EXPECT_EQ(&c, processors[2]->signal());
}