#pragma once

#include "IAudioPlayer.h"
#include "ISpatializedHearingAidSimulationFactory.h"
#include "PrescriptionReader.h"
#include "BrirReader.h"
#include "AudioFrameReader.h"
#include "AudioLoader.h"
#include "ICalibrationComputer.h"
#include "StimulusList.h"
#include "Documenter.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <presentation/Model.h>

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

class RefactoredModel : public Model {
	std::string nextStimulus_{};
	int framesPerBufferForTest{};
	std::shared_ptr<AudioFrameProcessorFactoryFactory> processorFactoryFactory;
	std::shared_ptr<AudioFrameProcessorFactory> processorFactoryForTest;
	StimulusList *list;
	Documenter *documenter;
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	AudioFrameReaderFactory *audioReaderFactory;
	IAudioPlayer *player;
	AudioLoader *loader;
public:
	SPATIALIZED_HA_SIMULATION_API RefactoredModel(
		StimulusList *,
		Documenter *,
		IAudioPlayer *,
		AudioLoader *,
		AudioFrameReaderFactory *,
		PrescriptionReader *,
		BrirReader *,
		ISpatializedHearingAidSimulationFactory *,
		ICalibrationComputerFactory *
	);
	SPATIALIZED_HA_SIMULATION_API void prepareNewTest(TestParameters *) override;
	SPATIALIZED_HA_SIMULATION_API void playNextTrial(TrialParameters *) override;
	SPATIALIZED_HA_SIMULATION_API bool testComplete() override;
	SPATIALIZED_HA_SIMULATION_API void playCalibration(CalibrationParameters *) override;
	void saveAudio(SaveAudioParameters *) override;
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
	void prepareAudioPlayer(AudioFrameReader &, int framesPerBuffer, std::string audioDevice);
	void prepareNewTest_(TestParameters *);
	std::shared_ptr<AudioFrameProcessorFactory> makeProcessorFactory(ProcessingParameters);
};