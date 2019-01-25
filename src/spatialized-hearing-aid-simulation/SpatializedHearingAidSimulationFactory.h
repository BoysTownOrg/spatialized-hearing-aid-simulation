#pragma once

#include <playing-audio/AudioFrameProcessor.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <dsl-prescription/ConfigurationFileParser.h>
#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <signal-processing/SignalProcessor.h>
#include <presentation/Presenter.h>

#ifdef SPATIALIZED_HA_SIMULATION_EXPORTS
	#define SPATIALIZED_HA_SIMULATION_API __declspec(dllexport)
#else
	#define SPATIALIZED_HA_SIMULATION_API __declspec(dllimport)
#endif

class SpatializedHearingAidSimulationFactory : public AudioFrameProcessorFactory {
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory;
	std::shared_ptr<PrescriptionReader> prescriptionReader;
	std::shared_ptr<BrirReader> brirReader;
	GlobalTestParameters global{};
public:
	SPATIALIZED_HA_SIMULATION_API SpatializedHearingAidSimulationFactory(
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
		std::shared_ptr<PrescriptionReader> prescriptionReader,
		std::shared_ptr<BrirReader> brirReader
	);
	std::shared_ptr<AudioFrameProcessor> make(Parameters) override;
	int preferredBufferSize() override;
	double fullScale_dB_Spl() override;
	void assertCanBeMade(GlobalTestParameters *) override;
	void storeParameters(GlobalTestParameters *) override;
private:
	BrirReader::BinauralRoomImpulseResponse readBrir(std::string);
	PrescriptionReader::Dsl readPrescription(std::string filePath);
	FilterbankCompressor::Parameters toCompressorParameters(
		GlobalTestParameters *,
		int sampleRate, 
		PrescriptionReader::Dsl
	);
	std::shared_ptr<SignalProcessor> makeFilter(std::vector<float> b);
	std::shared_ptr<SignalProcessor> makeHearingAid(
		FilterbankCompressor::Parameters
	);
};