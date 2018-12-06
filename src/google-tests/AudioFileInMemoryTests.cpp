#include "assert-utility.h"
#include "AudioFileReaderStub.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

class AudioFileInMemoryTestCase : public ::testing::TestCase {};

TEST(AudioFileInMemoryTestCase, readFillsEachChannel) {
	AudioFileReaderStub reader{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } };
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
	AudioFileReaderStub reader{ {} };
	AudioFileInMemory adapter{ reader };
}

TEST(AudioFileInMemoryTestCase, readNothingWhenExhausted) {
	AudioFileReaderStub reader{ { 3, 4 } };
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

TEST(AudioFileInMemoryTestCase, completeWhenExhausted) {
	AudioFileReaderStub reader{ { 3, 4 } };
	AudioFileInMemory adapter{ reader };
	float x{};
	float *channels[]{ &x };
	adapter.read(channels, 1);
	EXPECT_FALSE(adapter.complete());
	adapter.read(channels, 1);
	EXPECT_TRUE(adapter.complete());
}

TEST(AudioFileInMemoryTestCase, returnsFileParameters) {
	AudioFileReaderStub reader{};
	reader.setChannels(1);
	reader.setSampleRate(2);
	AudioFileInMemory adapter{ reader };
	EXPECT_EQ(1, adapter.channels());
	EXPECT_EQ(2, adapter.sampleRate());
}

TEST(AudioFileInMemoryTestCase, factoryThrowsCreateErrorOnFileError) {
	try {
		const auto reader = std::make_shared<AudioFileReaderStub>();
		reader->fail();
		reader->setErrorMessage("error.");
		AudioFileInMemoryFactory factory{ std::make_shared<AudioFileReaderStubFactory>(reader) };
		factory.make("");
		FAIL() << "Expected AudioFrameReaderFactory::CreateError";
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		assertEqual("error.", e.what());
	}
}

TEST(AudioFileInMemoryTestCase, factoryPassesFilePath) {
	const auto factory = std::make_shared<AudioFileReaderStubFactory>();
	AudioFileInMemoryFactory adapter{ factory };
	adapter.make("a");
	assertEqual("a", factory->filePath());
}
