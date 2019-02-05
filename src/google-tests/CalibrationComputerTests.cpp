#include "FakeAudioFileReader.h"
#include "AudioFrameReaderStub.h"
#include "assert-utility.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <spatialized-hearing-aid-simulation/CalibrationComputer.h>
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

TEST_F(CalibrationComputerTests, constructorResetsReader) {
	AudioFrameReaderStub reader{};
	CalibrationComputer computer{ reader };
	assertTrue(reader.readingLog().endsWith("reset "));
}