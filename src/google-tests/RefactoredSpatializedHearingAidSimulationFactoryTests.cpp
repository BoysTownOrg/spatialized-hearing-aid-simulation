#include <signal-processing/SignalProcessor.h>
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>

class RefactoredSpatializedHearingAidSimulationFactory {
	ScalarFactory *scalarFactory;
public:
	RefactoredSpatializedHearingAidSimulationFactory(ScalarFactory *scalarFactory) :
		scalarFactory{ scalarFactory } {}

	struct SimulationParameters {
		float scale;
	};
	std::shared_ptr<SignalProcessor> make(SimulationParameters p) {
		scalarFactory->make(p.scale);
		return {};
	}
};

#include <gtest/gtest.h>

namespace {
	class ScalarFactoryStub : public ScalarFactory {
		float scalar_{};
		std::shared_ptr<SignalProcessor> processor{};
	public:
		void setProcessor(std::shared_ptr<SignalProcessor> p) noexcept {
			processor = std::move(p);
		}

		auto scalar() const {
			return scalar_;
		}

		std::shared_ptr<SignalProcessor> make(float x) override {
			scalar_ = x;
			return processor;
		}
	};

	class RefactoredSpatializedHearingAidSimulationFactoryTests : public ::testing::Test {
	protected:
		RefactoredSpatializedHearingAidSimulationFactory::SimulationParameters simulationParameters;
		ScalarFactoryStub scalarFactory{};
		RefactoredSpatializedHearingAidSimulationFactory simulationFactory{ &scalarFactory };
	};

	TEST_F(RefactoredSpatializedHearingAidSimulationFactoryTests, makePassesScalarToFactory) {
		simulationParameters.scale = 1;
		simulationFactory.make(simulationParameters);
		EXPECT_EQ(1, scalarFactory.scalar());
	}

	TEST_F(
		RefactoredSpatializedHearingAidSimulationFactoryTests, 
		makePassesPrescriptionToHearingAidFactory
	) {
		simulationParameters.usingHearingAidSimulation = true;
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		simulationParameters.prescription = prescription;
		simulationFactory.make(simulationParameters);
		assertEqual({ 1 }, hearingAidFactory.parameters().compressionRatios);
		assertEqual({ 2 }, hearingAidFactory.parameters().crossFrequenciesHz);
		assertEqual({ 3 }, hearingAidFactory.parameters().kneepointGains_dB);
		assertEqual({ 4 }, hearingAidFactory.parameters().kneepoints_dBSpl);
		assertEqual({ 5 }, hearingAidFactory.parameters().broadbandOutputLimitingThresholds_dBSpl);
		EXPECT_EQ(6, hearingAidFactory.parameters().channels);
	}
}