#pragma once

#include "AudioPlayer.h"
#include "ISpatializedHearingAidSimulationFactory.h"
#include "PrescriptionReader.h"
#include "BrirReader.h"
#include "AudioFrameReader.h"
#include "AudioProcessingLoader.h"
#include "ICalibrationComputer.h"
#include "StimulusList.h"
#include "Documenter.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <common-includes/RuntimeError.h>
#include <presentation/Model.h>
#include <memory>
#include <string>

class AudioFrameWriter {
public:
	INTERFACE_OPERATIONS(AudioFrameWriter);
};

class AudioFrameWriterFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameWriterFactory);
	virtual std::shared_ptr<AudioFrameWriter> make(std::string filePath) = 0;
	RUNTIME_ERROR(CreateError);
};

class AudioFrameProcessorFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessorFactory);

	struct CommonHearingAidSimulation {
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	virtual std::shared_ptr<AudioFrameProcessor> make(
		AudioFrameReader *reader,
		double level_dB_Spl
	) = 0;
};

class AudioFrameProcessorFactoryFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameProcessorFactoryFactory);
	virtual std::shared_ptr<AudioFrameProcessorFactory> makeSpatialization(
		BrirReader::BinauralRoomImpulseResponse) = 0;

	virtual std::shared_ptr<AudioFrameProcessorFactory> makeHearingAid(
		AudioFrameProcessorFactory::CommonHearingAidSimulation,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_) = 0;

	virtual std::shared_ptr<AudioFrameProcessorFactory> makeFullSimulation(
		BrirReader::BinauralRoomImpulseResponse,
		AudioFrameProcessorFactory::CommonHearingAidSimulation,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_) = 0;

	virtual std::shared_ptr<AudioFrameProcessorFactory> makeNoSimulation() = 0;
};

class SpatialHearingAidModel : public Model {
	std::string nextStimulus_{};
	int framesPerBufferForTest{};
	std::shared_ptr<AudioFrameProcessorFactoryFactory> processorFactoryFactory;
	std::shared_ptr<AudioFrameProcessorFactory> processorFactoryForTest;
	StimulusList *stimulusList;
	Documenter *documenter;
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	AudioFrameReaderFactory *audioReaderFactory;
	AudioFrameWriterFactory *audioWriterFactory;
	AudioPlayer *player;
	AudioProcessingLoaderFactory *audioProcessingLoaderFactory;
public:
	SPATIALIZED_HA_SIMULATION_API SpatialHearingAidModel(
		StimulusList *,
		Documenter *,
		AudioPlayer *,
		AudioProcessingLoaderFactory *,
		AudioFrameReaderFactory *,
		AudioFrameWriterFactory *,
		PrescriptionReader *,
		BrirReader *,
		ISpatializedHearingAidSimulationFactory *,
		ICalibrationComputerFactory *
	);
	SPATIALIZED_HA_SIMULATION_API void prepareNewTest(Testing *) override;
	SPATIALIZED_HA_SIMULATION_API void playNextTrial(Trial *) override;
	SPATIALIZED_HA_SIMULATION_API bool testComplete() override;
	SPATIALIZED_HA_SIMULATION_API void playCalibration(Calibration *) override;
	SPATIALIZED_HA_SIMULATION_API void processAudioForSaving(SavingAudio *) override;
	SPATIALIZED_HA_SIMULATION_API void saveAudio(std::string) override;
	SPATIALIZED_HA_SIMULATION_API void stopCalibration() override;
	SPATIALIZED_HA_SIMULATION_API std::vector<std::string> audioDeviceDescriptions() override;
	SPATIALIZED_HA_SIMULATION_API static const double fullScaleLevel_dB_Spl;
	SPATIALIZED_HA_SIMULATION_API static const int defaultFramesPerBuffer;
private:
	void assertSizeIsPowerOfTwo(int);
	BrirReader::BinauralRoomImpulseResponse readAndCheckBrir(std::string filePath);
	PrescriptionReader::Dsl readPrescription(std::string filePath);
	BrirReader::BinauralRoomImpulseResponse readBrir(std::string filePath);
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<AudioFrameWriter> makeWriter(std::string filePath);
	void prepareAudioPlayer(AudioPlayer::Preparation);
	void prepareNewTest_(Testing *);
	std::shared_ptr<AudioFrameProcessorFactory> makeProcessorFactory(SignalProcessing);
};