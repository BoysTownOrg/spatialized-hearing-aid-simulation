#include "FakeAudioFile.h"
#include "AudioFrameReaderStub.h"
#include "assert-utility.h"
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <spatialized-hearing-aid-simulation/CalibrationComputer.h>
#include <gtest/gtest.h>

class CalibrationComputerTests : public ::testing::Test {
protected:
	CalibrationComputer construct(AudioFileReader &r) {
		return CalibrationComputer{ *std::make_shared<AudioFileInMemory>(r) };
	}
};

TEST_F(CalibrationComputerTests, computesSignalScaleMono) {
	FakeAudioFileReader reader{ { 1, 2, 3, 4, 5, 6 } };
	reader.setChannels(1);
	const auto channelRms = std::sqrt((1*1 + 2*2 + 3*3 + 4*4 + 5*5 + 6*6.0) / 6);
	auto computer = construct(reader);
	EXPECT_NEAR(std::pow(10.0, 7/20.0) / channelRms, computer.signalScale(0, 7), 1e-6);
}

TEST_F(CalibrationComputerTests, computesSignalScaleStereo) {
	FakeAudioFileReader reader{ { 1, 2, 3, 4, 5, 6 } };
	reader.setChannels(2);
	const auto leftChannelRms = std::sqrt((1*1 + 3*3 + 5*5.0) / 3);
	const auto rightChannelRms = std::sqrt((2*2 + 4*4 + 6*6.0) / 3);
	auto computer = construct(reader);
	EXPECT_NEAR(std::pow(10.0, 7/20.0) / leftChannelRms, computer.signalScale(0, 7), 1e-6);
	EXPECT_NEAR(std::pow(10.0, 8/20.0) / rightChannelRms, computer.signalScale(1, 8), 1e-6);
}

TEST_F(CalibrationComputerTests, constructorResetsReader) {
	AudioFrameReaderStub reader{};
	CalibrationComputer computer{ reader };
	assertTrue(reader.log().endsWith("reset "));
}

TEST_F(CalibrationComputerTests, outOfRangeChannelReturnsZero) {
	FakeAudioFileReader fakeReader{};
	fakeReader.setChannels(1);
	auto computer = construct(fakeReader);
	const auto firstChannel = 0;
	assertEqual(0.0, computer.signalScale(firstChannel + 1, {}));
}