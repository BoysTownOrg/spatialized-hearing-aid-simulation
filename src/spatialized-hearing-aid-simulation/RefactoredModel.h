#pragma once

#include "spatialized-hearing-aid-simulation-exports.h"
#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <audio-stream-processing/AudioFrameReader.h>
#include <recognition-test/StimulusPlayer.h>
#include <playing-audio/AudioPlayer.h>
#include <playing-audio/AudioLoader.h>
#include <presentation/Model.h>

class AudioStimulusPlayer : public IAudioPlayer, public StimulusPlayer {

};

class SpeechPerceptionTest {
public:
	INTERFACE_OPERATIONS(SpeechPerceptionTest);
	struct TestParameters {
		std::string audioDirectory;
		std::string testFilePath;
	};
	virtual void prepareNewTest(TestParameters) = 0;
	RUNTIME_ERROR(TestInitializationFailure);

	virtual void playNextTrial(StimulusPlayer *) = 0;
	virtual std::string nextStimulus() = 0;
};

class RefactoredModel : public Model {
	TestParameters testParameters{};
	PrescriptionReader* prescriptionReader;
	BrirReader *brirReader;
	SpeechPerceptionTest *test;
	FilterbankCompressorFactory *compressorFactory;
	AudioFrameReaderFactory *audioReaderFactory;
	AudioStimulusPlayer *player;
	AudioLoader *loader;
public:
	RefactoredModel(
		SpeechPerceptionTest *test,
		PrescriptionReader *prescriptionReader,
		BrirReader *brirReader,
		FilterbankCompressorFactory *compressorFactory,
		AudioFrameReaderFactory *audioReaderFactory,
		AudioStimulusPlayer *player,
		AudioLoader *loader
	) :
		prescriptionReader{ prescriptionReader },
		brirReader{ brirReader },
		test{ test },
		compressorFactory{ compressorFactory },
		audioReaderFactory{ audioReaderFactory },
		player{ player },
		loader{ loader }
	{
		player->setAudioLoader(loader);
	}

	SPATIALIZED_HA_SIMULATION_API void prepareNewTest(TestParameters p) override;
	SPATIALIZED_HA_SIMULATION_API void playTrial(TrialParameters p) override;
	SPATIALIZED_HA_SIMULATION_API bool testComplete() override;
	SPATIALIZED_HA_SIMULATION_API void playCalibration(CalibrationParameters) override;
	SPATIALIZED_HA_SIMULATION_API void stopCalibration() override;
	SPATIALIZED_HA_SIMULATION_API std::vector<std::string> audioDeviceDescriptions() override;

private:
	void readPrescriptions(Model::TestParameters p);
	PrescriptionReader::Dsl readPrescription(std::string filePath);
	BrirReader::BinauralRoomImpulseResponse readBrir(Model::TestParameters p);
	std::shared_ptr<FilterbankCompressor> makeCompressor(PrescriptionReader::Dsl dsl, int sampleRate);
	void prepareAudioPlayer(AudioFrameReader &reader, Model::TrialParameters p);
	void prepareNewTest_(Model::TestParameters p);
};