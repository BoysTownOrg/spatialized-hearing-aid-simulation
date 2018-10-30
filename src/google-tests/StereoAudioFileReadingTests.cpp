#include <audio-file-reading/StereoAudioFileInMemory.h>
#include <gtest/gtest.h>
#include <vector>

class MockAudioFileReader : public AudioFileReader {
	std::vector<float> contents;
	int _channels;
public:
	explicit MockAudioFileReader(
		std::vector<float> contents,
		int _channels = 1
	) :
		contents{ std::move(contents) },
		_channels(_channels) {}

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
	std::shared_ptr<StereoAudioFileInMemory> file;
public:
	AudioFileInMemoryFacade(
		AudioFileReader &reader
	) :
		file { std::make_shared<StereoAudioFileInMemory>(reader) } {}

	static AudioFileInMemoryFacade Stereo(std::vector<float> contents) {
		MockAudioFileReader reader{ std::move(contents), 2 };
		return AudioFileInMemoryFacade{ reader };
	}

	static AudioFileInMemoryFacade Signal(std::vector<float> contents) {
		MockAudioFileReader reader{ std::move(contents), 1 };
		return AudioFileInMemoryFacade{ reader };
	}

	int framesRemaining() const {
		return file->framesRemaining();
	}

	void readFrames(int frames) {
		std::vector<float> x(frames);
		file->read(&x[0], &x[0], frames);
	}

	void read(float *left, float *right, int frames) {
		file->read(left, right, frames);
	}
};

class AudioFileReadingTestCase : public ::testing::TestCase {};

TEST(AudioFileReadingTestCase, constructorThrowsIfNotSignalOrStereo) {
	MockAudioFileReader reader{ std::vector<float>{} };
	reader.setChannels(0);
	EXPECT_THROW(
		StereoAudioFileInMemory file{ reader }, 
		StereoAudioFileInMemory::InvalidChannelCount);
	reader.setChannels(3);
	EXPECT_THROW(
		StereoAudioFileInMemory file{ reader }, 
		StereoAudioFileInMemory::InvalidChannelCount);
}

TEST(AudioFileReadingTestCase, emptyFileHasZeroFramesRemaining) {
	const auto file = AudioFileInMemoryFacade::Signal({});
	EXPECT_EQ(0, file.framesRemaining());
}

TEST(AudioFileReadingTestCase, readReducesFramesRemaining) {
	auto file = AudioFileInMemoryFacade::Signal({ 1, 2, 3 });
	EXPECT_EQ(3, file.framesRemaining());
	file.readFrames(1);
	EXPECT_EQ(2, file.framesRemaining());
	file.readFrames(1);
	EXPECT_EQ(1, file.framesRemaining());
	file.readFrames(1);
	EXPECT_EQ(0, file.framesRemaining());
	file.readFrames(1);
	EXPECT_EQ(0, file.framesRemaining());
}

TEST(AudioFileReadingTestCase, readChannelsSampleBySample) {
	auto file = AudioFileInMemoryFacade::Stereo({ 3, 4, 5, 6 });
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
	auto file = AudioFileInMemoryFacade::Stereo({ 3, 4 });
	float left{};
	float right{};
	file.read(&left, &right, 1);
	file.read(&left, &right, 1);
	EXPECT_EQ(3, left);
	EXPECT_EQ(4, right);
}

TEST(AudioFileReadingTestCase, readLessThanRequested) {
	auto file = AudioFileInMemoryFacade::Stereo({ 3, 4 });
	float left[2]{};
	float right[2]{};
	file.read(left, right, 2);
	EXPECT_EQ(3, left[0]);
	EXPECT_EQ(4, right[0]);
	EXPECT_EQ(0, left[1]);
	EXPECT_EQ(0, right[1]);
}

TEST(AudioFileReadingTestCase, readSignalFileCopiesLeftChannelToRight) {
	auto file = AudioFileInMemoryFacade::Signal({ 3, 4 });
	float left{};
	float right{};
	file.read(&left, &right, 1);
	EXPECT_EQ(3, left);
	EXPECT_EQ(3, right);
	file.read(&left, &right, 1);
	EXPECT_EQ(4, left);
	EXPECT_EQ(4, right);
}
