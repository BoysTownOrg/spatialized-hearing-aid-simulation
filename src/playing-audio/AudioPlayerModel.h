#pragma once

#include <common-includes/Interface.h>
#include <string>

class SpatializedHearingAidSimulator {
public:
	struct Parameters {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		std::string audioFilePath;
		double level_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	INTERFACE_OPERATIONS(SpatializedHearingAidSimulator);
};

#include <memory>

class SpatializedHearingAidSimulatorFactory {
public:
	INTERFACE_OPERATIONS(SpatializedHearingAidSimulatorFactory);
	virtual std::shared_ptr<SpatializedHearingAidSimulator> make(
		SpatializedHearingAidSimulator::Parameters) = 0;
};

#include "playing-audio-exports.h"
#include <presentation/SpatializedHearingAidSimulationModel.h>
#include <audio-device-control/AudioDevice.h>

class AudioPlayerModel : public SpatializedHearingAidSimulationModel {
	std::shared_ptr<AudioDeviceFactory> deviceFactory;
	std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory;
public:
	PLAYING_AUDIO_API AudioPlayerModel(
		std::shared_ptr<AudioDeviceFactory>,
		std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory
	);
	PLAYING_AUDIO_API void playRequest(PlayRequest) override;
private:
	void throwIfNotDouble(std::string x, std::string identifier);
	void throwIfNotPositiveInteger(std::string x, std::string identifier);
	void throwRequestFailure(std::string x, std::string identifier);
};
