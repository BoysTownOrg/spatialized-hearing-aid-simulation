#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <audio-stream-processing/ChannelCopier.h>
#include <gtest/gtest.h>

class ChannelCopierTests : public ::testing::Test {
protected:
	std::shared_ptr<AudioFrameReaderStub> reader =
		std::make_shared<AudioFrameReaderStub>();
	ChannelCopier copier{ reader };
};

TEST_F(ChannelCopierTests, returnsCompleteWhenComplete) {
	EXPECT_FALSE(copier.complete());
	reader->setComplete();
	EXPECT_TRUE(copier.complete());
}

TEST_F(ChannelCopierTests, returnsParameters) {
	reader->setSampleRate(1);
	reader->setChannels(2);
	reader->setFrames(3);
	EXPECT_EQ(1, copier.sampleRate());
	EXPECT_EQ(2, copier.channels());
	EXPECT_EQ(3, copier.frames());
}

TEST_F(ChannelCopierTests, returnsTwoIfOneChannel) {
	reader->setChannels(1);
	EXPECT_EQ(2, copier.channels());
}

TEST_F(ChannelCopierTests, reset) {
	copier.reset();
	EXPECT_TRUE(reader->readingLog().contains("reset "));
}

TEST(ChannelCopierOtherTests, copiesFirstChannelToSecondWhenOnlyOneChannel) {
	FakeAudioFileReader reader{ std::vector<float>{ 1, 2, 3 } };
	ChannelCopier copier{ std::make_shared<AudioFileInMemory>(reader) };
	std::vector<float> left(3);
	std::vector<float> right(3);
	std::vector<gsl::span<float>> x{ left, right };
	copier.read(x);
	assertEqual({ 1, 2, 3 }, left);
	assertEqual({ 1, 2, 3 }, right);
}

TEST(ChannelCopierOtherTests, factoryPassesFilePath) {
	const auto factory = std::make_shared<AudioFrameReaderStubFactory>();
	ChannelCopierFactory adapter{ factory };
	adapter.make("a");
	assertEqual("a", factory->filePath());
}
