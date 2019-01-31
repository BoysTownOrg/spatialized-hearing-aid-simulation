#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <audio-stream-processing/ChannelCopier.h>
#include <gtest/gtest.h>

namespace {
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
		reader->setFrames(2);
		reader->setChannels(3);
		reader->setRemainingFrames(4);
		EXPECT_EQ(1, copier.sampleRate());
		EXPECT_EQ(2, copier.frames());
		EXPECT_EQ(3, copier.channels());
		EXPECT_EQ(4, copier.remainingFrames());
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
		FakeAudioFileReader reader;
		reader.setContents({ 1, 2, 3 });
		reader.setChannels(1);
		ChannelCopier copier{ std::make_shared<AudioFileInMemory>(reader) };
		std::vector<float> left(3);
		std::vector<float> right(3);
		std::vector<gsl::span<float>> stereo{ left, right };
		copier.read(stereo);
		assertEqual({ 1, 2, 3 }, left);
		assertEqual({ 1, 2, 3 }, right);
	}

	TEST(ChannelCopierOtherTests, factoryPassesFilePath) {
		AudioFrameReaderStubFactory factory;
		ChannelCopierFactory adapter{ &factory };
		adapter.make("a");
		assertEqual("a", factory.filePath());
	}
}
