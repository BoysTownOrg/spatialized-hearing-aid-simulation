#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <audio-stream-processing/ChannelCopier.h>
#include <gtest/gtest.h>

class ChannelCopierTestCase : public ::testing::TestCase {};

TEST(ChannelCopierTestCase, copiesFirstChannelToSecondWhenOnlyOneChannel) {
	FakeAudioFileReader reader{ std::vector<float>{ 1, 2, 3 } };
	ChannelCopier copier{ std::make_shared<AudioFileInMemory>(reader) };
	std::vector<float> left(3);
	std::vector<float> right(3);
	float *x[]{ &left[0], &right[0] };
	copier.read(x, 3);
	assertEqual({ 1, 2, 3 }, left);
	assertEqual({ 1, 2, 3 }, right);
}

TEST(ChannelCopierTestCase, returnsCompleteWhenComplete) {
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	ChannelCopier copier{ reader };
	EXPECT_FALSE(copier.complete());
	reader->setComplete();
	EXPECT_TRUE(copier.complete());
}

TEST(ChannelCopierTestCase, returnsParameters) {
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	reader->setSampleRate(1);
	reader->setChannels(2);
	reader->setFrames(3);
	ChannelCopier copier{ reader };
	EXPECT_EQ(1, copier.sampleRate());
	EXPECT_EQ(2, copier.channels());
	EXPECT_EQ(3, copier.frames());
}

TEST(ChannelCopierTestCase, returnsTwoIfOneChannel) {
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	reader->setChannels(1);
	ChannelCopier copier{ reader };
	EXPECT_EQ(2, copier.channels());
}

TEST(ChannelCopierTestCase, factoryPassesFilePath) {
	const auto factory = std::make_shared<AudioFrameReaderStubFactory>();
	ChannelCopierFactory adapter{ factory };
	adapter.make("a");
	assertEqual("a", factory->filePath());
}
