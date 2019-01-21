#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

class AudioFileInMemoryTests : public ::testing::Test {
protected:
	FakeAudioFileReader reader{};
	std::vector<float> mono{};
	std::vector<float> left{};
	std::vector<float> right{};

	void readStereo() {
		std::vector<gsl::span<float>> channels{ left, right };
		AudioFileInMemory adapter{ reader };
		adapter.read(channels);
	}
};

TEST_F(AudioFileInMemoryTests, readFillsEachChannelStereo) {
	reader.setContents({ 1, 2, 3, 4, 5, 6 });
	reader.setChannels(2);
	left.resize(3);
	right.resize(3);
	readStereo();
	assertEqual({ 1, 3, 5 }, left);
	assertEqual({ 2, 4, 6 }, right);
}

TEST_F(AudioFileInMemoryTests, emptyFileDoesNotThrowException) {
	reader.setContents({});
	AudioFileInMemory adapter{ reader };
}

TEST_F(AudioFileInMemoryTests, readNothingWhenExhausted) {
	reader.setContents({ 3, 4 });
	AudioFileInMemory adapter{ reader };
	mono.resize(1);
	std::vector<gsl::span<float>> channels{ mono };
	adapter.read(channels);
	EXPECT_EQ(3, mono.front());
	adapter.read(channels);
	EXPECT_EQ(4, mono.front());
	adapter.read(channels);
	EXPECT_EQ(4, mono.front());
}

TEST_F(AudioFileInMemoryTests, completeWhenExhausted) {
	reader.setContents({ 3, 4 });
	AudioFileInMemory adapter{ reader };
	mono.resize(1);
	std::vector<gsl::span<float>> channels{ mono };
	adapter.read(channels);
	EXPECT_FALSE(adapter.complete());
	adapter.read(channels);
	EXPECT_TRUE(adapter.complete());
}

TEST_F(AudioFileInMemoryTests, returnsFramesRemaining) {
	reader.setContents({ 1, 2, 3 });
    AudioFileInMemory adapter{ reader };
	mono.resize(1);
	std::vector<gsl::span<float>> channels{ mono };
    EXPECT_EQ(3, adapter.framesRemaining());
	adapter.read(channels);
    EXPECT_EQ(2, adapter.framesRemaining());
	adapter.read(channels);
    EXPECT_EQ(1, adapter.framesRemaining());
	adapter.read(channels);
    EXPECT_EQ(0, adapter.framesRemaining());
}

TEST_F(AudioFileInMemoryTests, returnsFramesRemainingStereo) {
	reader.setContents({ 1, 2, 3, 4, 5, 6 });
	reader.setChannels(2);
    AudioFileInMemory adapter{ reader };
	left.resize(1);
	right.resize(1);
    std::vector<gsl::span<float>> channels{ left, right };
    EXPECT_EQ(3, adapter.framesRemaining());
    adapter.read(channels);
    EXPECT_EQ(2, adapter.framesRemaining());
    adapter.read(channels);
    EXPECT_EQ(1, adapter.framesRemaining());
    adapter.read(channels);
    EXPECT_EQ(0, adapter.framesRemaining());
}

TEST_F(AudioFileInMemoryTests, returnsFileParameters) {
	reader.setContents({ 4, 5, 6 });
	reader.setChannels(3);
	reader.setSampleRate(2);
	AudioFileInMemory adapter{ reader };
	EXPECT_EQ(3, adapter.channels());
	EXPECT_EQ(2, adapter.sampleRate());
	EXPECT_EQ(1, adapter.frames());
}

TEST_F(AudioFileInMemoryTests, seeksBeginningOnReset) {
	reader.setContents({ 3, 4 });
	AudioFileInMemory adapter{ reader };
	mono.resize(1);
	std::vector<gsl::span<float>> channels{ mono };
    adapter.read(channels);
	EXPECT_EQ(3, mono.front());
    adapter.read(channels);
	EXPECT_EQ(4, mono.front());
	adapter.reset();
    adapter.read(channels);
	EXPECT_EQ(3, mono.front());
    adapter.read(channels);
	EXPECT_EQ(4, mono.front());
}

class AudioFileInMemoryFactoryTests : public ::testing::Test {
protected:
	std::shared_ptr<FakeAudioFileReader> reader = 
		std::make_shared<FakeAudioFileReader>();
	std::shared_ptr<FakeAudioFileReaderFactory> factory = 
		std::make_shared<FakeAudioFileReaderFactory>(reader);
	AudioFileInMemoryFactory adapter{ factory };

	void assertMakeThrowsCreateError(std::string what) {
		try {
			make();
			FAIL() << "Expected AudioFrameReaderFactory::CreateError";
		}
		catch (const AudioFrameReaderFactory::CreateError &e) {
			assertEqual(what, e.what());
		}
	}

	void make(std::string f = {}) {
		adapter.make(std::move(f));
	}
};

TEST_F(AudioFileInMemoryFactoryTests, factoryThrowsCreateErrorOnFileError) {
	reader->fail();
	reader->setErrorMessage("error.");
	assertMakeThrowsCreateError("error.");
}

TEST_F(AudioFileInMemoryFactoryTests, factoryPassesFilePath) {
	make("a");
	assertEqual("a", factory->filePath());
}
