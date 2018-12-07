#pragma once

#include "playing-audio-exports.h"
#include "AudioFrameProcessor.h"
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <dsl-prescription/ConfigurationFileParser.h>
#include <dsl-prescription/DslPrescription.h>
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <signal-processing/SignalProcessor.h>

class SpatializedHearingAidSimulationFactory : public AudioFrameProcessorFactory {
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory;
	std::shared_ptr<ConfigurationFileParserFactory> parserFactory;
public:
	PLAYING_AUDIO_API SpatializedHearingAidSimulationFactory(
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
		std::shared_ptr<ConfigurationFileParserFactory> parserFactory
	);
	std::shared_ptr<AudioFrameProcessor> make(Parameters) override;
private:
	BrirReader::BinauralRoomImpulseResponse makeBrir(std::string filePath);
	std::shared_ptr<SignalProcessor> makeChannel(
		std::vector<float> b,
		std::string filePath,
		FilterbankCompressor::Parameters,
		double rms,
		double level_dB_Spl);
	DslPrescription makeDslPrescription(std::string filePath);
	std::shared_ptr<SignalProcessor> makeFilter(std::vector<float> b);
	std::shared_ptr<SignalProcessor> makeHearingAid(
		const DslPrescription &,
		FilterbankCompressor::Parameters
	);
};