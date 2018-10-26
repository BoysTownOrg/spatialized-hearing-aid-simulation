#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>
#include <vector>

class MockAudioFileReader : public AudioFileReader {
	std::vector<float> contents;
	int _channels = 1;
public:
	explicit MockAudioFileReader(std::vector<float> contents) :
		contents{ std::move(contents) } {}

	void setChannels(int c) {
		_channels = c;
	}

	long long frames() override
	{
		return _channels == 0 ? 0 : contents.size() / _channels;
	}

	int channels() override
	{
		return _channels;
	}

	void readFrames(float *x, long long n) override
	{
		if (contents.size() == 0)
			return;
		std::memcpy(
			x,
			&contents[0], 
			static_cast<std::size_t>(n) * sizeof(float) * _channels);
	}
};

class AudioFileInMemoryFacade {
	AudioFileInMemory file;
public:
	explicit AudioFileInMemoryFacade(std::shared_ptr<AudioFileReader> reader) :
		file{ std::move(reader) } {}

	static AudioFileInMemoryFacade Stereo(std::vector<float> contents) {
		const auto reader =
			std::make_shared<MockAudioFileReader>(std::move(contents));
		reader->setChannels(2);
		return AudioFileInMemoryFacade{ std::move(reader) };
	}

	static AudioFileInMemoryFacade Mono(std::vector<float> contents) {
		const auto reader =
			std::make_shared<MockAudioFileReader>(std::move(contents));
		reader->setChannels(1);
		return AudioFileInMemoryFacade{ std::move(reader) };
	}

	int framesRemaining() {
		return file.framesRemaining();
	}
};

class AudioFileReadingTestCase : public ::testing::TestCase {};

TEST(AudioFileReadingTestCase, constructorThrowsIfNotMonoOrStereo) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{});
	reader->setChannels(0);
	EXPECT_THROW(
		AudioFileInMemory file{ reader }, 
		AudioFileInMemory::InvalidChannelCount);
	reader->setChannels(3);
	EXPECT_THROW(
		AudioFileInMemory file{ reader }, 
		AudioFileInMemory::InvalidChannelCount);
}

TEST(AudioFileReadingTestCase, emptyFileHasZeroFramesRemaining) {
	auto file = AudioFileInMemoryFacade::Mono({});
	EXPECT_EQ(0, file.framesRemaining());
}

TEST(AudioFileReadingTestCase, readReducesFramesRemaining) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 1, 2, 3 });
	AudioFileInMemory file{ reader };
	EXPECT_EQ(3, file.framesRemaining());
	float x{};
	file.read(&x, &x, 1);
	EXPECT_EQ(2, file.framesRemaining());
	file.read(&x, &x, 1);
	EXPECT_EQ(1, file.framesRemaining());
	file.read(&x, &x, 1);
	EXPECT_EQ(0, file.framesRemaining());
	file.read(&x, &x, 1);
	EXPECT_EQ(0, file.framesRemaining());
}

TEST(AudioFileReadingTestCase, readChannelsSampleBySample) {
	const auto reader = 
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4, 5, 6 });
	reader->setChannels(2);
	AudioFileInMemory file{ reader };
	float left{};
	float right{};
	file.read(&left, &right, 1);
	EXPECT_EQ(3, left);
	EXPECT_EQ(4, right);
	file.read(&left, &right, 1);
	EXPECT_EQ(5, left);
	EXPECT_EQ(6, right);
}

TEST(AudioFileReadingTestCase, readNothingWhenExhausted) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4 });
	reader->setChannels(2);
	AudioFileInMemory file{ reader };
	float left{};
	float right{};
	file.read(&left, &right, 1);
	file.read(&left, &right, 1);
	EXPECT_EQ(3, left);
	EXPECT_EQ(4, right);
}

TEST(AudioFileReadingTestCase, readLessThanRequested) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4 });
	reader->setChannels(2);
	AudioFileInMemory file{ reader };
	float left[2]{};
	float right[2]{};
	file.read(left, right, 2);
	EXPECT_EQ(3, left[0]);
	EXPECT_EQ(4, right[0]);
	EXPECT_EQ(0, left[1]);
	EXPECT_EQ(0, right[1]);
}

TEST(AudioFileReadingTestCase, readMonoFileCopiesLeftChannelToRight) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4 });
	reader->setChannels(1);
	AudioFileInMemory file{ reader };
	float left{};
	float right{};
	file.read(&left, &right, 1);
	EXPECT_EQ(3, left);
	EXPECT_EQ(3, right);
	file.read(&left, &right, 1);
	EXPECT_EQ(4, left);
	EXPECT_EQ(4, right);
}
