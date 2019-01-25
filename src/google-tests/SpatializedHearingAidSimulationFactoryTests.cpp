#include "assert-utility.h"
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
		setValidDefaults();
	}

private:
	void setValidDefaults() {
		global.usingHearingAidSimulation = false;
		global.usingSpatialization = false;
		parser->setValidSingleChannelDslProperties();
		compressor->setChunkSize(1);
		compressor->setWindowSize(1);
		audioFileReader->setContents({ 0, 0 });
		audioFileReader->setChannels(2);
	}

public:
	void canNotBeMade(std::string what) {
		try {
			factory.assertCanBeMade(&global);
		}
		catch (const SpatializedHearingAidSimulationFactory::CreateError &e) {
			assertEqual(std::move(what), e.what());
		}
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
	assertCanBeMadeDoesNotTryToMakePrescriptionsWhenNotUsingHearingAidSimulation
) {
	global.leftDslPrescriptionFilePath = "a";
	global.rightDslPrescriptionFilePath = "b";
	global.usingHearingAidSimulation = false;
	factory.assertCanBeMade(&global);
	EXPECT_TRUE(parserFactory->filePaths().empty());
}

TEST_F(
	SpatializedHearingAidSimulationTests, 
	assertCanBeMadePassesCompressionParametersToFactory
) {
	global.usingHearingAidSimulation = true;
	global.attack_ms = 1;
	global.release_ms = 2;
	global.chunkSize = 4;
	global.windowSize = 8;
	factory.assertCanBeMade(&global);
	EXPECT_EQ(1, compressorFactory->parameters().attack_ms);
	EXPECT_EQ(2, compressorFactory->parameters().release_ms);
	EXPECT_EQ(4, compressorFactory->parameters().chunkSize);
	EXPECT_EQ(8, compressorFactory->parameters().windowSize);
}

TEST_F(
	SpatializedHearingAidSimulationTests, 
	assertCanBeMadePassesBrirFilePathToAudioFileReaderFactory
) {
	global.brirFilePath = "a";
	global.usingSpatialization = true;
	factory.assertCanBeMade(&global);
	assertEqual("a", audioFileReaderFactory->filePath());
}

TEST_F(
	SpatializedHearingAidSimulationTests, 
	assertCanBeMadeDoesNotTryToMakeBrirWhenNotUsingSpatialization
) {
	global.brirFilePath = "a";
	global.usingSpatialization = false;
	factory.assertCanBeMade(&global);
	EXPECT_TRUE(audioFileReaderFactory->filePath().empty());
}

TEST_F(
	SpatializedHearingAidSimulationTests, 
	assertCanBeMadeThrowsCreateErrorWhenFilterCoefficientsEmpty
) {
	audioFileReader->setContents({});
	global.usingSpatialization = true;
	canNotBeMade("Invalid filter coefficients.");
}

TEST_F(
	SpatializedHearingAidSimulationTests, 
	assertCanBeMadeThrowsCreateErrorWhenWindowSizeNotPowerOfTwo
) {
	compressor->setWindowSize(3);
	global.usingHearingAidSimulation = true;
	canNotBeMade("The window size must be a power of two.");
}

TEST_F(
	SpatializedHearingAidSimulationTests, 
	assertCanBeMadeThrowsCreateErrorWhenChunkSizeNotPowerOfTwo
) {
	compressor->setChunkSize(3);
	global.usingHearingAidSimulation = true;
	canNotBeMade("The chunk size must be a power of two.");
}