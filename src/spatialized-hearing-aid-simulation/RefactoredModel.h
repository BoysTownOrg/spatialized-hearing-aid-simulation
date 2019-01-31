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

class FirFilterFactory {
public:
	INTERFACE_OPERATIONS(FirFilterFactory);
	virtual std::shared_ptr<SignalProcessor> make(BrirReader::impulse_response_type) = 0;
};

class HearingAidFactory {
public:
	INTERFACE_OPERATIONS(HearingAidFactory);
	virtual std::shared_ptr<SignalProcessor> make(
		FilterbankCompressor::Parameters
	) = 0;
};

class ScalarFactory {
public:
	INTERFACE_OPERATIONS(ScalarFactory);
	virtual std::shared_ptr<SignalProcessor> make(float) = 0;
};

class RefactoredModel : public Model {
	PrescriptionReader::Dsl leftPrescription{};
	PrescriptionReader::Dsl rightPrescription{};
	BrirReader::BinauralRoomImpulseResponse brir{};
	TestParameters testParameters{};
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	SpeechPerceptionTest *perceptionTest;
	HearingAidFactory *hearingAidFactory;
	FirFilterFactory *firFilterFactory;
	ScalarFactory *scalarFactory;
	AudioFrameReaderFactory *audioReaderFactory;
	IAudioPlayer *player;
	AudioLoader *loader;
public:
	SPATIALIZED_HA_SIMULATION_API RefactoredModel(
		SpeechPerceptionTest *perceptionTest,
		IAudioPlayer *player,
		AudioLoader *loader,
		AudioFrameReaderFactory *audioReaderFactory,
		HearingAidFactory *hearingAidFactory,
		PrescriptionReader *prescriptionReader,
		FirFilterFactory *firFilterFactory,
		BrirReader *brirReader,
		ScalarFactory *scalarFactory
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
	std::shared_ptr<SignalProcessor> makeHearingAid(PrescriptionReader::Dsl, int sampleRate);
	void prepareAudioPlayer(AudioFrameReader &, TrialParameters);
	void prepareNewTest_(TestParameters);
};