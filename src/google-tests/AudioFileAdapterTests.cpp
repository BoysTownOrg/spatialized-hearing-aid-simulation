#include <audio-file-reading/AudioFileReader.h>
#include <audio-stream-processing/AudioFrameReader.h>
#include <gsl/gsl>
#include <vector>

class AudioFileAdapter : public AudioFrameReader {
	using size_type = std::vector<float>::size_type;
	std::vector<float> buffer;
	int channelCount;
	size_type head = 0;
public:
	explicit AudioFileAdapter(
		std::shared_ptr<AudioFileReader> reader
	) :
		buffer(gsl::narrow<size_type>(reader->frames() * reader->channels())),
		channelCount(reader->channels())
	{
		reader->readFrames(&buffer[0], reader->frames());
	}

	void read(float ** channels, int frameCount) override {
		for (int i = 0; i < frameCount; ++i)
			for (int j = 0; j < channelCount; ++j)
				channels[j][i] = buffer[head++];
	}
};

#include "assert-utility.h"
#include "MockAudioFileReader.h"
#include <gtest/gtest.h>

class AudioFileAdapterTestCase : public ::testing::TestCase {};

TEST(AudioFileAdapterTestCase, tbd) {
	const auto reader = std::make_shared<MockAudioFileReader>(
		std::vector<float>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });
	reader->setChannels(3);
	AudioFileAdapter adapter{ reader };
	std::vector<float> a(4);
	std::vector<float> b(4);
	std::vector<float> c(4);
	float *channels[] { &a[0], &b[0], &c[0] };
	adapter.read(channels, 4);
	assertEqual({ 1, 4, 7, 10 }, a);
	assertEqual({ 2, 5, 8, 11 }, b);
	assertEqual({ 3, 6, 9, 12 }, c);
}