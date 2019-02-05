#pragma once

#include "SpeechPerceptionTest.h"
#include "IAudioPlayer.h"
#include "ISpatializedHearingAidSimulationFactory.h"
#include "PrescriptionReader.h"
#include "BrirReader.h"
#include "AudioFrameReader.h"
#include "AudioLoader.h"
#include "ICalibrationComputer.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <presentation/Model.h>

class RefactoredModel : public Model {
	PrescriptionReader::Dsl leftPrescription{};
	PrescriptionReader::Dsl rightPrescription{};
	BrirReader::BinauralRoomImpulseResponse brir{};
	TestParameters testParameters{};
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	SpeechPerceptionTest *perceptionTest;
	AudioFrameReaderFactory *audioReaderFactory;
	IAudioPlayer *player;
	AudioLoader *loader;
	ISpatializedHearingAidSimulationFactory *simulationFactory;
	ICalibrationComputerFactory *calibrationFactory;
public:
	SPATIALIZED_HA_SIMULATION_API RefactoredModel(
		SpeechPerceptionTest *perceptionTest,
		IAudioPlayer *player,
		AudioLoader *loader,
		AudioFrameReaderFactory *audioReaderFactory,
		PrescriptionReader *prescriptionReader,
		BrirReader *brirReader,
		ISpatializedHearingAidSimulationFactory *simulationFactory,
		ICalibrationComputerFactory *calibrationFactory
	);
	SPATIALIZED_HA_SIMULATION_API void prepareNewTest(TestParameters) override;
	SPATIALIZED_HA_SIMULATION_API void playTrial(TrialParameters) override;
	SPATIALIZED_HA_SIMULATION_API bool testComplete() override;
	SPATIALIZED_HA_SIMULATION_API void playCalibration(CalibrationParameters) override;
	SPATIALIZED_HA_SIMULATION_API void stopCalibration() override;
	SPATIALIZED_HA_SIMULATION_API std::vector<std::string> audioDeviceDescriptions() override;
	SPATIALIZED_HA_SIMULATION_API static const double fullScaleLevel_dB_Spl;
	SPATIALIZED_HA_SIMULATION_API static const int defaultFramesPerBuffer;
private:
	void checkAndStore(TestParameters);
	void checkAndStorePrescriptions(TestParameters);
	void checkSizeIsPowerOfTwo(int);
	void checkAndStoreBrir(TestParameters);
	void readPrescriptions(TestParameters);
	PrescriptionReader::Dsl readPrescription(std::string filePath);
	BrirReader::BinauralRoomImpulseResponse readBrir(std::string filePath);
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	void prepareAudioPlayer(AudioFrameReader &, ProcessingParameters, std::string audioDevice);
	void prepareNewTest_(TestParameters);
	void play_(
		BrirReader::BinauralRoomImpulseResponse brir_,
		PrescriptionReader::Dsl leftPrescription_,
		PrescriptionReader::Dsl rightPrescription_,
		std::string audioFilePath,
		ProcessingParameters processing,
		double level_dB_Spl_,
		std::string audioDevice_
	);
};