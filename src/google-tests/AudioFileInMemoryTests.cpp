#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

TEST(AudioFileInMemoryTests, readFillsEachChannel) {
	FakeAudioFileReader reader{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } };
	reader.setChannels(3);
	AudioFileInMemory adapter{ reader };
	std::vector<float> a(4);
	std::vector<float> b(4);
	std::vector<float> c(4);
	std::vector<gsl::span<float>> channels{ a, b, c };
	adapter.read(channels);
	assertEqual({ 1, 4, 7, 10 }, a);
	assertEqual({ 2, 5, 8, 11 }, b);
	assertEqual({ 3, 6, 9, 12 }, c);
}

TEST(AudioFileInMemoryTests, emptyFileDoesNotThrowException) {
	FakeAudioFileReader reader{ {} };
	AudioFileInMemory adapter{ reader };
}

TEST(AudioFileInMemoryTests, readNothingWhenExhausted) {
	FakeAudioFileReader reader{ { 3, 4 } };
	AudioFileInMemory adapter{ reader };
	float x{};
	gsl::span<float> channels{ &x, 1 };
	adapter.read({ &channels, 1 });
	EXPECT_EQ(3, x);
	adapter.read({ &channels, 1 });
	EXPECT_EQ(4, x);
	adapter.read({ &channels, 1 });
	EXPECT_EQ(4, x);
}

TEST(AudioFileInMemoryTests, completeWhenExhausted) {
	FakeAudioFileReader reader{ { 3, 4 } };
	AudioFileInMemory adapter{ reader };
	float x{};
	gsl::span<float> channels{ &x, 1 };
	adapter.read({ &channels, 1 });
	EXPECT_FALSE(adapter.complete());
	adapter.read({ &channels, 1 });
	EXPECT_TRUE(adapter.complete());
}

TEST(AudioFileInMemoryTests, returnsFramesRemaining) {
    FakeAudioFileReader reader{ { 1, 2, 3 } };
    AudioFileInMemory adapter{ reader };
    float x{};
    gsl::span<float> channels{ &x, 1 };
    EXPECT_EQ(3, adapter.framesRemaining());
    adapter.read({ &channels, 1 });
    EXPECT_EQ(2, adapter.framesRemaining());
    adapter.read({ &channels, 1 });
    EXPECT_EQ(1, adapter.framesRemaining());
    adapter.read({ &channels, 1 });
    EXPECT_EQ(0, adapter.framesRemaining());
}

TEST(AudioFileInMemoryTests, returnsFileParameters) {
	FakeAudioFileReader reader{ { 4, 5, 6 } };
	reader.setChannels(3);
	reader.setSampleRate(2);
	AudioFileInMemory adapter{ reader };
	EXPECT_EQ(3, adapter.channels());
	EXPECT_EQ(2, adapter.sampleRate());
	EXPECT_EQ(1, adapter.frames());
}

TEST(AudioFileInMemoryTests, seeksBeginningOnReset) {
	FakeAudioFileReader reader{ { 3, 4 } };
	AudioFileInMemory adapter{ reader };
	float x{};
	gsl::span<float> channels{ &x, 1 };
	adapter.read({ &channels, 1 });
	EXPECT_EQ(3, x);
	adapter.read({ &channels, 1 });
	EXPECT_EQ(4, x);
	adapter.reset();
	adapter.read({ &channels, 1 });
	EXPECT_EQ(3, x);
	adapter.read({ &channels, 1 });
	EXPECT_EQ(4, x);
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
