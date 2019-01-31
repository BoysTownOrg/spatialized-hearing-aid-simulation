#include <signal-processing/SignalProcessor.h>
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>

class RefactoredSpatializedHearingAidSimulationFactory {
	ScalarFactory *scalarFactory;
	HearingAidFactory *hearingAidFactory;
public:
	RefactoredSpatializedHearingAidSimulationFactory(
		ScalarFactory *scalarFactory,
		HearingAidFactory *hearingAidFactory
	) :
		scalarFactory{ scalarFactory },
		hearingAidFactory{ hearingAidFactory } {}

	struct SimulationParameters {
		PrescriptionReader::Dsl prescription;
		float scale;
		bool usingHearingAidSimulation;
	};
	std::shared_ptr<SignalProcessor> make(SimulationParameters p) {
		scalarFactory->make(p.scale);
		FilterbankCompressor::Parameters compression;
		compression.compressionRatios = p.prescription.compressionRatios;
		compression.crossFrequenciesHz = p.prescription.crossFrequenciesHz;
		compression.kneepointGains_dB = p.prescription.kneepointGains_dB;
		compression.kneepoints_dBSpl = p.prescription.kneepoints_dBSpl;
		compression.broadbandOutputLimitingThresholds_dBSpl = 
			p.prescription.broadbandOutputLimitingThresholds_dBSpl;
		compression.channels = p.prescription.channels;
		hearingAidFactory->make(compression);
		return {};
	}
};

#include "assert-utility.h"
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

	class HearingAidFactoryStub : public HearingAidFactory {
		FilterbankCompressor::Parameters parameters_{};
		std::shared_ptr<SignalProcessor> processor{};
	public:
		void setProcessor(std::shared_ptr<SignalProcessor> p) noexcept {
			processor = std::move(p);
		}

		auto parameters() const {
			return parameters_;
		}

		std::shared_ptr<SignalProcessor> make(FilterbankCompressor::Parameters p) override {
			parameters_ = std::move(p);
			return processor;
		}
	};

	class RefactoredSpatializedHearingAidSimulationFactoryTests : public ::testing::Test {
	protected:
		RefactoredSpatializedHearingAidSimulationFactory::SimulationParameters simulationParameters;
		ScalarFactoryStub scalarFactory{};
		HearingAidFactoryStub hearingAidFactory{};
		RefactoredSpatializedHearingAidSimulationFactory simulationFactory{ 
			&scalarFactory, 
			&hearingAidFactory 
		};
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