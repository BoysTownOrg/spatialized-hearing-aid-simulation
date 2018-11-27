#include "assert-utility.h"
#include "MockAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

class AudioFileInMemoryTestCase : public ::testing::TestCase {};

TEST(AudioFileInMemoryTestCase, readFillsEachChannel) {
	MockAudioFileReader reader{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } };
	reader.setChannels(3);
	AudioFileInMemory adapter{ reader };
	std::vector<float> a(4);
	std::vector<float> b(4);
	std::vector<float> c(4);
	float *channels[] { &a[0], &b[0], &c[0] };
	adapter.read(channels, 4);
	assertEqual({ 1, 4, 7, 10 }, a);
	assertEqual({ 2, 5, 8, 11 }, b);
	assertEqual({ 3, 6, 9, 12 }, c);
}

TEST(AudioFileInMemoryTestCase, emptyFileDoesNotThrowException) {
	MockAudioFileReader reader{ {} };
	AudioFileInMemory adapter{ reader };
}

TEST(AudioFileInMemoryTestCase, factoryThrowsFileError) {
	try {
		const auto reader = std::make_shared<MockAudioFileReader>(std::vector<float>{});
		reader->setFailedTrue();
		reader->setErrorMessage("error.");
		const auto mockFactory = std::make_shared<MockAudioFileReaderFactory>(reader);
		AudioFileInMemoryFactory factory{ mockFactory };
		factory.make("");
		FAIL() << "Expected AudioFileInMemory::FileError.";
	}
	catch (const AudioFrameReaderFactory::FileError &e) {
		assertEqual("error.", e.what());
	}
}

TEST(AudioFileInMemoryTestCase, readNothingWhenExhausted) {
	MockAudioFileReader reader{ { 3, 4 } };
	AudioFileInMemory adapter{ reader };
	float x{};
	float *channels[] { &x };
	adapter.read(channels, 1);
	EXPECT_EQ(3, x);
	adapter.read(channels, 1);
	EXPECT_EQ(4, x);
	adapter.read(channels, 1);
	EXPECT_EQ(4, x);
}