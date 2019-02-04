#include <spatialized-hearing-aid-simulation/AudioFrameReader.h>

class CalibrationComputer {
public:
	explicit CalibrationComputer(AudioFrameReader &) {}
	AudioFrameReader::channel_type::element_type signalScale(int channel, double level) {
		channel;
		level;
		return {};
	}
};

#include "FakeAudioFileReader.h"
#include "AudioFrameReaderStub.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

class CalibrationComputerTests : public ::testing::Test {
};

TEST_F(CalibrationComputerTests, computesSignalScaleForTwoChannels) {
	FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
	fakeReader.setChannels(2);
	const auto leftChannelRms = std::sqrt((1 * 1 + 3 * 3 + 5 * 5.0) / 3);
	const auto rightChannelRms = std::sqrt((2 * 2 + 4 * 4 + 6 * 6.0) / 3);
	CalibrationComputer computer{ *std::make_shared<AudioFileInMemory>(fakeReader) };
	EXPECT_NEAR(std::pow(10.0, 7 / 20.0) / leftChannelRms, computer.signalScale(0, 7), 1e-6);
	EXPECT_NEAR(std::pow(10.0, 8 / 20.0) / rightChannelRms, computer.signalScale(1, 8), 1e-6);
}