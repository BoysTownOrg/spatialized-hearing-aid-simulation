#pragma once

#include "AudioPlayer.h"
#include "SimulationChannelFactory.h"
#include "PrescriptionReader.h"
#include "BrirReader.h"
#include "AudioFrameReader.h"
#include "AudioFrameWriter.h"
#include "AudioProcessingLoader.h"
#include "CalibrationComputer.h"
#include "StimulusList.h"
#include "TestDocumenter.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <presentation/Model.h>
#include <memory>
#include <string>

class StereoSimulationFactory {
public:
    INTERFACE_OPERATIONS(StereoSimulationFactory)

	struct HearingAidSimulation {
		PrescriptionReader::Dsl leftPrescription;
		PrescriptionReader::Dsl rightPrescription;
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
    INTERFACE_OPERATIONS(AudioFrameProcessorFactoryFactory)
	virtual std::shared_ptr<StereoSimulationFactory> makeSpatialization(
		BrirReader::BinauralRoomImpulseResponse
	) = 0;

	virtual std::shared_ptr<StereoSimulationFactory> makeHearingAid(
		StereoSimulationFactory::HearingAidSimulation
	) = 0;

	virtual std::shared_ptr<StereoSimulationFactory> makeFullSimulation(
		BrirReader::BinauralRoomImpulseResponse,
		StereoSimulationFactory::HearingAidSimulation
	) = 0;

	virtual std::shared_ptr<StereoSimulationFactory> makeNoSimulation() = 0;
};

class SpatialHearingAidModel : public Model {
	std::vector<std::vector<AudioFrameWriter::channel_type::element_type>> toWrite_{};
    AudioFrameWriter::AudioFormat formatToWrite_{};
	std::string nextStimulus_{};
	std::shared_ptr<AudioFrameProcessorFactoryFactory> processorFactoryFactory;
	std::shared_ptr<StereoSimulationFactory> processorFactoryForTest;
	StimulusList *stimulusList;
	TestDocumenter *documenter;
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	AudioFrameReaderFactory *audioReaderFactory;
	AudioFrameWriterFactory *audioWriterFactory;
	AudioPlayer *player;
	AudioProcessingLoaderFactory *audioProcessingLoaderFactory;
	int framesPerBufferForTest{};
public:
	SPATIALIZED_HA_SIMULATION_API SpatialHearingAidModel(
		StimulusList *,
		TestDocumenter *,
		AudioPlayer *,
		AudioProcessingLoaderFactory *,
		AudioFrameReaderFactory *,
		AudioFrameWriterFactory *,
		PrescriptionReader *,
		BrirReader *,
		SimulationChannelFactory *,
		CalibrationComputerFactory *
	);
	SPATIALIZED_HA_SIMULATION_API void prepareNewTest(const Testing &) override;
	SPATIALIZED_HA_SIMULATION_API void playNextTrial(const Trial &) override;
	SPATIALIZED_HA_SIMULATION_API bool testComplete() override;
	SPATIALIZED_HA_SIMULATION_API void playCalibration(const Calibration &) override;
	SPATIALIZED_HA_SIMULATION_API void processAudioForSaving(const SavingAudio &) override;
	SPATIALIZED_HA_SIMULATION_API void saveAudio(std::string) override;
	SPATIALIZED_HA_SIMULATION_API void stopCalibration() override;
	SPATIALIZED_HA_SIMULATION_API std::vector<std::string> audioDeviceDescriptions() override;
	SPATIALIZED_HA_SIMULATION_API static const double fullScaleLevel_dB_Spl;
	SPATIALIZED_HA_SIMULATION_API static const int defaultFramesPerBuffer;
private:
	struct PlayAudioRequest {
		std::string audioFilePath;
		std::string audioDevice;
		double level_dB_Spl;
		int framesPerBuffer;
		StereoSimulationFactory *processorFactory;
	};
	void playAudio(const PlayAudioRequest &);
	
	struct MakeAudioLoader {
		double level_dB_Spl;
		std::shared_ptr<AudioFrameReader> reader;
		StereoSimulationFactory *processorFactory;
	};
	std::shared_ptr<AudioLoader>makeLoader(const MakeAudioLoader &);

	void assertSizeIsPowerOfTwo(int);
	int framesPerBuffer(const SignalProcessing &);
	BrirReader::BinauralRoomImpulseResponse readAndCheckBrir(std::string filePath);
	PrescriptionReader::Dsl readPrescription(std::string filePath);
	BrirReader::BinauralRoomImpulseResponse readBrir(std::string filePath);
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<AudioFrameWriter> makeWriter(std::string filePath);
	void prepareAudioPlayer(const AudioPlayer::Preparation &);
	void prepareNewTest_(const Testing &);
	std::shared_ptr<StereoSimulationFactory> makeProcessorFactory(const SignalProcessing &);
	StereoSimulationFactory::HearingAidSimulation hearingAidSimulation(const SignalProcessing &);
};
