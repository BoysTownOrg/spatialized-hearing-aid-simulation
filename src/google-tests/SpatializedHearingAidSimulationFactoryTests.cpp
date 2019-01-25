#include "FakeAudioFileReader.h"
#include "FakeConfigurationFileParser.h"
#include "FilterbankCompressorSpy.h"
#include <dsl-prescription/PrescriptionAdapter.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <spatialized-hearing-aid-simulation/SpatializedHearingAidSimulationFactory.h>
#include <gtest/gtest.h>

class SpatializedHearingAidSimulationTests : public ::testing::Test {
protected:
	GlobalTestParameters global;
	std::shared_ptr<FilterbankCompressorSpy> compressor =
		std::make_shared<FilterbankCompressorSpy>();
	std::shared_ptr<FilterbankCompressorSpyFactory> compressorFactory =
		std::make_shared<FilterbankCompressorSpyFactory>(compressor);
	std::shared_ptr<FakeConfigurationFileParser> parser =
		std::make_shared<FakeConfigurationFileParser>();
	std::shared_ptr<FakeConfigurationFileParserFactory> parserFactory =
		std::make_shared<FakeConfigurationFileParserFactory>(parser);
	std::shared_ptr<PrescriptionReader> prescriptionReader =
		std::make_shared<PrescriptionAdapter>(parserFactory);
	std::shared_ptr<FakeAudioFileReader> audioFileReader =
		std::make_shared<FakeAudioFileReader>();
	std::shared_ptr<FakeAudioFileReaderFactory> audioFileReaderFactory =
		std::make_shared<FakeAudioFileReaderFactory>(audioFileReader);
	std::shared_ptr<BrirReader> brirReader =
		std::make_shared<BrirAdapter>(audioFileReaderFactory);
	SpatializedHearingAidSimulationFactory factory{
		compressorFactory, 
		prescriptionReader, 
		brirReader
	};

	SpatializedHearingAidSimulationTests() {
		global.usingHearingAidSimulation = false;
		global.usingSpatialization = false;
		parser->setValidSingleChannelDslProperties();
		compressor->setChunkSize(1);
		compressor->setWindowSize(1);
	}
};

TEST_F(
	SpatializedHearingAidSimulationTests, 
	assertCanBeMadePassesPrescriptionFilePathsToParserFactory
) {
	global.leftDslPrescriptionFilePath = "a";
	global.rightDslPrescriptionFilePath = "b";
	global.usingHearingAidSimulation = true;
	factory.assertCanBeMade(&global);
	EXPECT_TRUE(parserFactory->filePaths().contains("a"));
	EXPECT_TRUE(parserFactory->filePaths().contains("b"));
}

TEST_F(
	SpatializedHearingAidSimulationTests, 
	assertCanBeMadeDoesTryToMakePrescriptionsWhenNotUsingHearingAidSimulation
) {
	global.leftDslPrescriptionFilePath = "a";
	global.rightDslPrescriptionFilePath = "b";
	global.usingHearingAidSimulation = false;
	factory.assertCanBeMade(&global);
	EXPECT_TRUE(parserFactory->filePaths().empty());
}