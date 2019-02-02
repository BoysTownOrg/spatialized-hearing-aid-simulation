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
		using buffer_type = std::vector<SignalProcessor::signal_type::element_type>;

		SpatializedHearingAidSimulationFactory::Spatialization spatialization;
		SpatializedHearingAidSimulationFactory::HearingAidSimulation hearingAidSimulation;
		SpatializedHearingAidSimulationFactory::FullSimulation fullSimulation;
		ScalarFactoryStub scalarFactory{};
		FirFilterFactoryStub firFilterFactory{};
		HearingAidFactoryStub hearingAidFactory{};
		SpatializedHearingAidSimulationFactory simulationFactory{ 
			&scalarFactory, 
			&firFilterFactory,
			&hearingAidFactory 
		};
	};

	TEST_F(SpatializedHearingAidSimulationFactoryTests, makeWithoutSimulationPassesScalarToFactory) {
		simulationFactory.makeWithoutSimulation(1);
		assertEqual(1.0f, scalarFactory.scalar());
	}

	TEST_F(SpatializedHearingAidSimulationFactoryTests, makeSpatializationPassesScalarToFactory) {
		simulationFactory.makeSpatialization({}, 1);
		assertEqual(1.0f, scalarFactory.scalar());
	}

	TEST_F(SpatializedHearingAidSimulationFactoryTests, makeHearingAidSimulationPassesScalarToFactory) {
		simulationFactory.makeHearingAidSimulation({}, 1);
		assertEqual(1.0f, scalarFactory.scalar());
	}

	TEST_F(SpatializedHearingAidSimulationFactoryTests, makeFullSimulationPassesScalarToFactory) {
		simulationFactory.makeFullSimulation({}, 1);
		assertEqual(1.0f, scalarFactory.scalar());
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
		makeHearingAidSimulationPassesPrescriptionToHearingAidFactory
	) {
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		hearingAidSimulation.prescription = prescription;
		simulationFactory.makeHearingAidSimulation(hearingAidSimulation, {});
		assertEqual({ 1 }, hearingAidFactory.parameters().compressionRatios);
		assertEqual({ 2 }, hearingAidFactory.parameters().crossFrequenciesHz);
		assertEqual({ 3 }, hearingAidFactory.parameters().kneepointGains_dB);
		assertEqual({ 4 }, hearingAidFactory.parameters().kneepoints_dBSpl);
		assertEqual({ 5 }, hearingAidFactory.parameters().broadbandOutputLimitingThresholds_dBSpl);
		assertEqual(6, hearingAidFactory.parameters().channels);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests,
		makeFullSimulationPassesPrescriptionToHearingAidFactory
	) {
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		fullSimulation.hearingAidSimulation.prescription = prescription;
		simulationFactory.makeFullSimulation(fullSimulation, {});
		assertEqual({ 1 }, hearingAidFactory.parameters().compressionRatios);
		assertEqual({ 2 }, hearingAidFactory.parameters().crossFrequenciesHz);
		assertEqual({ 3 }, hearingAidFactory.parameters().kneepointGains_dB);
		assertEqual({ 4 }, hearingAidFactory.parameters().kneepoints_dBSpl);
		assertEqual({ 5 }, hearingAidFactory.parameters().broadbandOutputLimitingThresholds_dBSpl);
		assertEqual(6, hearingAidFactory.parameters().channels);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
		makeHearingAidSimulationPassesCompressionParametersToHearingAidFactory
	) {
		hearingAidSimulation.attack_ms = 1;
		hearingAidSimulation.release_ms = 2;
		hearingAidSimulation.chunkSize = 3;
		hearingAidSimulation.windowSize = 4;
		hearingAidSimulation.sampleRate = 5;
		hearingAidSimulation.fullScaleLevel_dB_Spl = 6;
		simulationFactory.makeHearingAidSimulation(hearingAidSimulation, {});
		assertEqual(1.0, hearingAidFactory.parameters().attack_ms);
		assertEqual(2.0, hearingAidFactory.parameters().release_ms);
		assertEqual(3, hearingAidFactory.parameters().chunkSize);
		assertEqual(4, hearingAidFactory.parameters().windowSize);
		assertEqual(5.0, hearingAidFactory.parameters().sampleRate);
		assertEqual(6.0, hearingAidFactory.parameters().max_dB_Spl);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests,
		makeFullSimulationPassesCompressionParametersToHearingAidFactory
	) {
		fullSimulation.hearingAidSimulation.attack_ms = 1;
		fullSimulation.hearingAidSimulation.release_ms = 2;
		fullSimulation.hearingAidSimulation.chunkSize = 3;
		fullSimulation.hearingAidSimulation.windowSize = 4;
		fullSimulation.hearingAidSimulation.sampleRate = 5;
		fullSimulation.hearingAidSimulation.fullScaleLevel_dB_Spl = 6;
		simulationFactory.makeFullSimulation(fullSimulation, {});
		assertEqual(1.0, hearingAidFactory.parameters().attack_ms);
		assertEqual(2.0, hearingAidFactory.parameters().release_ms);
		assertEqual(3, hearingAidFactory.parameters().chunkSize);
		assertEqual(4, hearingAidFactory.parameters().windowSize);
		assertEqual(5.0, hearingAidFactory.parameters().sampleRate);
		assertEqual(6.0, hearingAidFactory.parameters().max_dB_Spl);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests, 
		makeSpatializationPassesCoefficientsToFirFilterFactory
	) {
		spatialization.filterCoefficients = { 1 };
		simulationFactory.makeSpatialization(spatialization, {});
		assertEqual({ 1 }, firFilterFactory.coefficients());
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests,
		makeFullSimulationPassesCoefficientsToFirFilterFactory
	) {
		fullSimulation.spatialization.filterCoefficients = { 1 };
		simulationFactory.makeFullSimulation(fullSimulation, {});
		assertEqual({ 1 }, firFilterFactory.coefficients());
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests,
		makeFullSimulationCombinesProcessorsInOrder
	) {
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared<AddsSamplesBy>(3.0f));
		auto processor = simulationFactory.makeFullSimulation(fullSimulation, {});
		buffer_type x{ 4 };
		processor->process(x);
		assertEqual({ (4 + 1) * 2 + 3.0f }, x);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests,
		makeHearingAidSimulationCombinesProcessorsInOrder
	) {
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared<AddsSamplesBy>(3.0f));
		auto processor = simulationFactory.makeHearingAidSimulation({}, {});
		buffer_type x{ 4 };
		processor->process(x);
		assertEqual({ 4 + 1 + 3.0f }, x);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests,
		makeSpatializationCombinesProcessorsInOrder
	) {
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared<AddsSamplesBy>(3.0f));
		auto processor = simulationFactory.makeSpatialization({}, {});
		buffer_type x{ 4 };
		processor->process(x);
		assertEqual({ (4 + 1) * 2.0f }, x);
	}

	TEST_F(
		SpatializedHearingAidSimulationFactoryTests,
		makeWithoutSimulationCombinesProcessorsInOrder
	) {
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared<AddsSamplesBy>(3.0f));
		auto processor = simulationFactory.makeWithoutSimulation({});
		buffer_type x{ 4 };
		processor->process(x);
		assertEqual({ 4 + 1 }, x);
	}
}