#pragma once

#include "SpeechPerceptionTest.h"
#include "IAudioPlayer.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <audio-stream-processing/AudioFrameReader.h>
#include <playing-audio/AudioLoader.h>
#include <signal-processing/SignalProcessor.h>
#include <presentation/Model.h>

class IRefactoredSpatializedHearingAidSimulationFactory {
public:
	INTERFACE_OPERATIONS(IRefactoredSpatializedHearingAidSimulationFactory);
	struct SimulationParameters {
		PrescriptionReader::Dsl prescription;
		BrirReader::impulse_response_type filterCoefficients;
		double attack_ms;
		double release_ms;
		double fullScaleLevel_dB_Spl;
		float scale;
		int sampleRate;
		int windowSize;
		int chunkSize;
		bool usingHearingAidSimulation;
		bool usingSpatialization;
	};
	virtual std::shared_ptr<SignalProcessor> make(SimulationParameters p) = 0;
};

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
	IRefactoredSpatializedHearingAidSimulationFactory *simulationFactory;
public:
	SPATIALIZED_HA_SIMULATION_API RefactoredModel(
		SpeechPerceptionTest *perceptionTest,
		IAudioPlayer *player,
		AudioLoader *loader,
		AudioFrameReaderFactory *audioReaderFactory,
		PrescriptionReader *prescriptionReader,
		BrirReader *brirReader,
		IRefactoredSpatializedHearingAidSimulationFactory *simulationFactory
	);
	SPATIALIZED_HA_SIMULATION_API void prepareNewTest(TestParameters) override;
	SPATIALIZED_HA_SIMULATION_API void playTrial(TrialParameters) override;
	SPATIALIZED_HA_SIMULATION_API bool testComplete() override;
	SPATIALIZED_HA_SIMULATION_API void playCalibration(CalibrationParameters) override;
	SPATIALIZED_HA_SIMULATION_API void stopCalibration() override;
	SPATIALIZED_HA_SIMULATION_API std::vector<std::string> audioDeviceDescriptions() override;
	SPATIALIZED_HA_SIMULATION_API static const double fullScaleLevel_dB_Spl;

private:
	void checkAndStore(TestParameters);
	void checkAndStorePrescriptions(TestParameters);
	void checkSizeIsPowerOfTwo(int);
	void checkAndStoreBrir(TestParameters);
	void readPrescriptions(TestParameters);
	PrescriptionReader::Dsl readPrescription(std::string filePath);
	BrirReader::BinauralRoomImpulseResponse readBrir(TestParameters);
	void prepareAudioPlayer(AudioFrameReader &, TrialParameters);
	void prepareNewTest_(TestParameters);
};