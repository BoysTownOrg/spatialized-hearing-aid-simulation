#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

class AudioFileInMemoryFacade {
	AudioFileInMemory inMemory;
public:
	std::vector<float> left{};
	std::vector<float> right{};

	AudioFileInMemoryFacade(AudioFileReader &reader) :
		inMemory{ reader } {}

	void readMono() {
		std::vector<gsl::span<float>> mono{ left };
		inMemory.read(mono);
	}

	void readStereo() {
		std::vector<gsl::span<float>> stereo{ left, right };
		inMemory.read(stereo);
	}

	bool complete() {
		return inMemory.complete();
	}

	long long framesRemaining() {
		return inMemory.framesRemaining();
	}

	void reset() {
		inMemory.reset();
	}
};

class AudioFileInMemoryTests : public ::testing::Test {
protected:
	FakeAudioFileReader reader{};
};

TEST_F(AudioFileInMemoryTests, readFillsEachChannelStereo) {
	reader.setContents({ 1, 2, 3, 4, 5, 6 });
	reader.setChannels(2);
	AudioFileInMemoryFacade adapter{ reader };
	adapter.left.resize(3);
	adapter.right.resize(3);
	adapter.readStereo();
	assertEqual({ 1, 3, 5 }, adapter.left);
	assertEqual({ 2, 4, 6 }, adapter.right);
}

TEST_F(AudioFileInMemoryTests, emptyFileDoesNotThrowException) {
	reader.setContents({});
	AudioFileInMemory adapter{ reader };
}

TEST_F(AudioFileInMemoryTests, readNothingWhenExhausted) {
	reader.setContents({ 3, 4 });
	AudioFileInMemoryFacade adapter{ reader };
	adapter.left.resize(1);
	adapter.readMono();
	EXPECT_EQ(3, adapter.left.front());
	adapter.readMono();
	EXPECT_EQ(4, adapter.left.front());
	adapter.readMono();
	EXPECT_EQ(4, adapter.left.front());
}

TEST_F(AudioFileInMemoryTests, completeWhenExhausted) {
	reader.setContents({ 3, 4 });
	AudioFileInMemoryFacade adapter{ reader };
	adapter.left.resize(1);
	adapter.readMono();
	EXPECT_FALSE(adapter.complete());
	adapter.readMono();
	EXPECT_TRUE(adapter.complete());
}

TEST_F(AudioFileInMemoryTests, returnsFramesRemaining) {
	reader.setContents({ 1, 2, 3 });
    AudioFileInMemoryFacade adapter{ reader };
	adapter.left.resize(1);
    EXPECT_EQ(3, adapter.framesRemaining());
	adapter.readMono();
    EXPECT_EQ(2, adapter.framesRemaining());
	adapter.readMono();
    EXPECT_EQ(1, adapter.framesRemaining());
	adapter.readMono();
    EXPECT_EQ(0, adapter.framesRemaining());
}

TEST_F(AudioFileInMemoryTests, returnsFramesRemainingStereo) {
	reader.setContents({ 1, 2, 3, 4, 5, 6 });
	reader.setChannels(2);
    AudioFileInMemoryFacade adapter{ reader };
	adapter.left.resize(1);
	adapter.right.resize(1);
    EXPECT_EQ(3, adapter.framesRemaining());
	adapter.readStereo();
    EXPECT_EQ(2, adapter.framesRemaining());
	adapter.readStereo();
    EXPECT_EQ(1, adapter.framesRemaining());
	adapter.readStereo();
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
	AudioFileInMemoryFacade adapter{ reader };
	adapter.left.resize(1);
	adapter.readMono();
	EXPECT_EQ(3, adapter.left.front());
	adapter.readMono();
	EXPECT_EQ(4, adapter.left.front());
	adapter.reset();
	adapter.readMono();
	EXPECT_EQ(3, adapter.left.front());
	adapter.readMono();
	EXPECT_EQ(4, adapter.left.front());
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
