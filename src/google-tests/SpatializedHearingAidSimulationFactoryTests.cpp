#include "FakeAudioFileReader.h"
#include "FakeConfigurationFileParser.h"
#include "FilterbankCompressorSpy.h"
#include <dsl-prescription/PrescriptionAdapter.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <spatialized-hearing-aid-simulation/SpatializedHearingAidSimulationFactory.h>
#include <gtest/gtest.h>

class SpatializedHearingAidSimulationTests : public ::testing::Test {
protected:
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory =
		std::make_shared<FilterbankCompressorSpyFactory>();
	std::shared_ptr<PrescriptionReader> prescriptionReader =
		std::make_shared<PrescriptionAdapter>(
			std::make_shared<FakeConfigurationFileParserFactory>()
		);
	std::shared_ptr<BrirReader> brirReader =
		std::make_shared<BrirAdapter>(
			std::make_shared<FakeAudioFileReaderFactory>()
		);
	SpatializedHearingAidSimulationFactory factory{compressorFactory, prescriptionReader, brirReader};
};

TEST_F(SpatializedHearingAidSimulationTests, tbd) {
	FAIL();
}