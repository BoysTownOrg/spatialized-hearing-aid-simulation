#include <signal-processing/SignalProcessor.h>
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>
#include <signal-processing/SignalProcessingChain.h>

class FirFilterFactory {
public:
	INTERFACE_OPERATIONS(FirFilterFactory);
	virtual std::shared_ptr<SignalProcessor> make(BrirReader::impulse_response_type) = 0;
};

class HearingAidFactory {
public:
	INTERFACE_OPERATIONS(HearingAidFactory);
	virtual std::shared_ptr<SignalProcessor> make(
		FilterbankCompressor::Parameters
	) = 0;
};

class ScalarFactory {
public:
	INTERFACE_OPERATIONS(ScalarFactory);
	virtual std::shared_ptr<SignalProcessor> make(float) = 0;
};

class RefactoredSpatializedHearingAidSimulationFactory {
	ScalarFactory *scalarFactory;
	FirFilterFactory *firFilterFactory;
	HearingAidFactory *hearingAidFactory;
public:
	RefactoredSpatializedHearingAidSimulationFactory(
		ScalarFactory *scalarFactory,
		FirFilterFactory *firFilterFactory,
		HearingAidFactory *hearingAidFactory
	) :
		scalarFactory{ scalarFactory },
		firFilterFactory{ firFilterFactory },
		hearingAidFactory{ hearingAidFactory } {}

	struct SimulationParameters {
		PrescriptionReader::Dsl prescription;
		BrirReader::impulse_response_type filterCoefficients;
		double attack_ms;
		double release_ms;
		double fullScaleLevel_dB_Spl;
		float scale;
		int sampleRate;
		int windowSize;
		int chunkSize;
		bool usingHearingAidSimulation;
		bool usingSpatialization;
	};
	std::shared_ptr<SignalProcessor> make(SimulationParameters p) {
		auto chain = std::make_shared<SignalProcessingChain>();
		chain->add(scalarFactory->make(p.scale));
		if (p.usingSpatialization)
			chain->add(firFilterFactory->make(p.filterCoefficients));
		if (p.usingHearingAidSimulation)
			chain->add(hearingAidFactory->make(compression(p)));
		return chain;
	}

	FilterbankCompressor::Parameters compression(SimulationParameters p) {
		FilterbankCompressor::Parameters compression_;
		compression_.compressionRatios = p.prescription.compressionRatios;
		compression_.crossFrequenciesHz = p.prescription.crossFrequenciesHz;
		compression_.kneepointGains_dB = p.prescription.kneepointGains_dB;
		compression_.kneepoints_dBSpl = p.prescription.kneepoints_dBSpl;
		compression_.broadbandOutputLimitingThresholds_dBSpl = 
			p.prescription.broadbandOutputLimitingThresholds_dBSpl;
		compression_.channels = p.prescription.channels;
		compression_.attack_ms = p.attack_ms;
		compression_.release_ms = p.release_ms;
		compression_.chunkSize = p.chunkSize;
		compression_.windowSize = p.windowSize;
		compression_.sampleRate = p.sampleRate;
		compression_.max_dB_Spl = p.fullScaleLevel_dB_Spl;
		return compression_;
	}
};

#include "assert-utility.h"
#include "SignalProcessorStub.h"
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

	class FirFilterFactoryStub : public FirFilterFactory {
		BrirReader::impulse_response_type coefficients_{};
		std::shared_ptr<SignalProcessor> processor{};
	public:
		void setProcessor(std::shared_ptr<SignalProcessor> p) noexcept {
			processor = std::move(p);
		}

		auto coefficients() const {
			return coefficients_;
		}

		std::shared_ptr<SignalProcessor> make(BrirReader::impulse_response_type b) override {
			coefficients_ = std::move(b);
			return processor;
		}
	};

	class RefactoredSpatializedHearingAidSimulationFactoryTests : public ::testing::Test {
	protected:
		RefactoredSpatializedHearingAidSimulationFactory::SimulationParameters simulationParameters;
		ScalarFactoryStub scalarFactory{};
		FirFilterFactoryStub firFilterFactory{};
		HearingAidFactoryStub hearingAidFactory{};
		RefactoredSpatializedHearingAidSimulationFactory simulationFactory{ 
			&scalarFactory, 
			&firFilterFactory,
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

	TEST_F(
		RefactoredSpatializedHearingAidSimulationFactoryTests, 
		makePassesOtherCompressionParametersToHearingAidFactory
	) {
		simulationParameters.usingHearingAidSimulation = true;
		simulationParameters.attack_ms = 1;
		simulationParameters.release_ms = 2;
		simulationParameters.chunkSize = 3;
		simulationParameters.windowSize = 4;
		simulationParameters.sampleRate = 5;
		simulationParameters.fullScaleLevel_dB_Spl = 6;
		simulationFactory.make(simulationParameters);
		EXPECT_EQ(1, hearingAidFactory.parameters().attack_ms);
		EXPECT_EQ(2, hearingAidFactory.parameters().release_ms);
		EXPECT_EQ(3, hearingAidFactory.parameters().chunkSize);
		EXPECT_EQ(4, hearingAidFactory.parameters().windowSize);
		EXPECT_EQ(5, hearingAidFactory.parameters().sampleRate);
		EXPECT_EQ(6, hearingAidFactory.parameters().max_dB_Spl);
	}

	TEST_F(
		RefactoredSpatializedHearingAidSimulationFactoryTests, 
		makePassesCoefficientsToFirFilterFactory
	) {
		simulationParameters.usingSpatialization = true;
		simulationParameters.filterCoefficients = { 1 };
		simulationFactory.make(simulationParameters);
		assertEqual({ 1 }, firFilterFactory.coefficients());
	}

	TEST_F(
		RefactoredSpatializedHearingAidSimulationFactoryTests, 
		makeCombinesProcessorsInOrder
	) {
		simulationParameters.usingSpatialization = true;
		simulationParameters.usingHearingAidSimulation = true;
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared<AddsSamplesBy>(3.0f));
		auto processor = simulationFactory.make(simulationParameters);
		std::vector<float> x{ 4 };
		processor->process(x);
		EXPECT_EQ((4 + 1) * 2 + 3, x.front());
	}

	TEST_F(
		RefactoredSpatializedHearingAidSimulationFactoryTests, 
		makeCombinesProcessorsInOrderWithoutSpatialization
	) {
		simulationParameters.usingSpatialization = false;
		simulationParameters.usingHearingAidSimulation = true;
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared<AddsSamplesBy>(3.0f));
		auto processor = simulationFactory.make(simulationParameters);
		std::vector<float> x{ 4 };
		processor->process(x);
		EXPECT_EQ(4 + 1 + 3, x.front());
	}

	TEST_F(
		RefactoredSpatializedHearingAidSimulationFactoryTests, 
		makeCombinesProcessorsInOrderWithoutHearingAidSimulation
	) {
		simulationParameters.usingSpatialization = true;
		simulationParameters.usingHearingAidSimulation = false;
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared<AddsSamplesBy>(3.0f));
		auto processor = simulationFactory.make(simulationParameters);
		std::vector<float> x{ 4 };
		processor->process(x);
		EXPECT_EQ((4 + 1) * 2, x.front());
	}
}