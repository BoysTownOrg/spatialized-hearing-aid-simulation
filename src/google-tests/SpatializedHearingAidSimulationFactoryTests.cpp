#include "assert-utility.h"
#include "SignalProcessorStub.h"
#include <spatialized-hearing-aid-simulation/SpatializedHearingAidSimulationFactory.h>
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

	class SpatializedHearingAidSimulationFactoryTests : public ::testing::Test {
	protected:
		SpatializedHearingAidSimulationFactory::SimulationParameters simulationParameters;
		ScalarFactoryStub scalarFactory{};
		FirFilterFactoryStub firFilterFactory{};
		HearingAidFactoryStub hearingAidFactory{};
		SpatializedHearingAidSimulationFactory simulationFactory{ 
			&scalarFactory, 
			&firFilterFactory,
			&hearingAidFactory 
		};
	};

	TEST_F(SpatializedHearingAidSimulationFactoryTests, makePassesScalarToFactory) {
		simulationParameters.scale = 1;
		simulationFactory.make(simulationParameters);
		assertEqual(1.0f, scalarFactory.scalar());
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
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
		assertEqual(6, hearingAidFactory.parameters().channels);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
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
		assertEqual(1.0, hearingAidFactory.parameters().attack_ms);
		assertEqual(2.0, hearingAidFactory.parameters().release_ms);
		assertEqual(3, hearingAidFactory.parameters().chunkSize);
		assertEqual(4, hearingAidFactory.parameters().windowSize);
		assertEqual(5.0, hearingAidFactory.parameters().sampleRate);
		assertEqual(6.0, hearingAidFactory.parameters().max_dB_Spl);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
		makePassesCoefficientsToFirFilterFactory
	) {
		simulationParameters.usingSpatialization = true;
		simulationParameters.filterCoefficients = { 1 };
		simulationFactory.make(simulationParameters);
		assertEqual({ 1 }, firFilterFactory.coefficients());
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
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
		assertEqual((4 + 1) * 2 + 3.0f, x.front());
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
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
		assertEqual(4 + 1 + 3.0f, x.front());
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
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
		assertEqual((4 + 1) * 2.0f, x.front());
	}
}