#pragma once

#include <playing-audio/AudioFrameProcessor.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <dsl-prescription/ConfigurationFileParser.h>
#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <signal-processing/SignalProcessor.h>

class SpatializedHearingAidSimulationFactory : public AudioProcessor {
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory;
	std::shared_ptr<PrescriptionReader> prescriptionReader;
	std::shared_ptr<BrirReader> brirReader;
public:
	SpatializedHearingAidSimulationFactory(
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
		std::shared_ptr<PrescriptionReader> prescriptionReader,
		std::shared_ptr<BrirReader> brirReader
	);
	std::shared_ptr<RefactoredAudioFrameProcessor> make(Initialization) override;
private:
	BrirReader::BinauralRoomImpulseResponse readBrir(std::string);
	std::shared_ptr<SignalProcessor> makeChannel(
		std::vector<float> b,
		FilterbankCompressor::Parameters,
		double
	);
	PrescriptionReader::Dsl readPrescription(std::string filePath);
	FilterbankCompressor::Parameters toCompressorParameters(Initialization, PrescriptionReader::Dsl);
	std::shared_ptr<SignalProcessor> makeFilter(std::vector<float> b);
	std::shared_ptr<SignalProcessor> makeHearingAid(
		FilterbankCompressor::Parameters
	);
};