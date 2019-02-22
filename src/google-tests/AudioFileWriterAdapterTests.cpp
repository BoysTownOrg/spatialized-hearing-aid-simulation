#include "assert-utility.h"
#include <audio-file-reading-writing/AudioFileWriterAdapter.h>
#include <gtest/gtest.h>
#include <vector>

class FakeAudioFileWriter : public AudioFileWriter {
	std::vector<float> written_{};
public:
	auto written() {
		return written_;
	}

	void writeFrames(float *x, long long n) override {
		gsl::span<float> audio{ x, gsl::narrow<gsl::span<float>::index_type>(n) };
		std::copy(audio.begin(), audio.end(), std::back_inserter(written_));
	}
};

namespace {
	class AudioFileWriterAdapterTests : public ::testing::Test {
	protected:
		std::shared_ptr<FakeAudioFileWriter> writer = 
			std::make_shared<FakeAudioFileWriter>();
		AudioFileWriterAdapter adapter{ writer };
	};

	TEST_F(AudioFileWriterAdapterTests, writeInterleavesChannels) {
		std::vector<float> left{ 1, 3, 5 };
		std::vector<float> right{ 2, 4, 6 };
		std::vector<gsl::span<float>> channels = { left, right };
		adapter.write(channels);
		assertEqual({ 1, 2, 3, 4, 5, 6 }, writer->written());
	}
}