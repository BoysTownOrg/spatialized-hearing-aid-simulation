
#include <common-includes/Interface.h>
#include <memory>

class AudioFileWriter {
public:
	INTERFACE_OPERATIONS(AudioFileWriter);
	virtual void writeFrames(float *, long long) = 0;
};

#include <gsl/gsl>

class AudioFileWriterAdapter {
	std::shared_ptr<AudioFileWriter> writer;
public:
	explicit AudioFileWriterAdapter(std::shared_ptr<AudioFileWriter> writer) :
		writer{ std::move(writer) } {}
	
	using channel_type = gsl::span<float>;

	void write(gsl::span<channel_type> audio) {
		std::vector<float> buffer{};
		for (int i = 0; i < audio.begin()->size(); ++i)
			for (auto channel : audio)
				buffer.push_back(channel.at(i));
		writer->writeFrames(&buffer.front(), buffer.size());
	}
};

#include "assert-utility.h"
#include <gtest/gtest.h>
#include <vector>

class FakeAudioFileWriter : public AudioFileWriter {
	std::vector<float> written_{};
public:
	auto written() {
		return written_;
	}

	void writeFrames(float *x, long long n) override {
		for (int i = 0; i < n; ++i)
			written_.push_back(x[i]);
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