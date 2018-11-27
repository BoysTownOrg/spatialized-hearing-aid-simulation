#include <audio-file-reading/AudioFileReader.h>
#include <audio-stream-processing/AudioFrameReader.h>

class AudioFileAdapter : public AudioFrameReader {
	std::shared_ptr<AudioFileReader> reader;
public:
	explicit AudioFileAdapter(std::shared_ptr<AudioFileReader> reader) : reader{ std::move(reader) } {}
	void read(float ** channels, int frameCount) override {
		channels;
		frameCount;
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
	std::vector<float> a(3);
	std::vector<float> b(3);
	std::vector<float> c(3);
	float *channels[] { &a[0], &b[0], &c[0] };
	adapter.read(channels, 4);
	assertEqual({ 1, 4, 7, 10 }, a);
	assertEqual({ 2, 5, 8, 11 }, b);
	assertEqual({ 3, 6, 9, 12 }, c);
}