#include "assert-utility.h"
#include "ArgumentCollection.h"
#include "LogString.h"
#include "FilterbankCompressorSpy.h"
#include "AudioFrameReaderStub.h"
#include "AudioLoaderStub.h"
#include "PrescriptionReaderStub.h"
#include "BrirReaderStub.h"
#include "FakeAudioFileReader.h"
#include "SignalProcessorStub.h"
#include "AudioPlayerStub.h"
#include "SpeechPerceptionTestStub.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>
#include <gtest/gtest.h>

namespace {
	class SpatializedHearingAidSimulationFactoryStub : 
		public ISpatializedHearingAidSimulationFactory 
	{
		ArgumentCollection<SimulationParameters> parameters_{};
		ArgumentCollection<FullSimulation> fullSimulation_{};
		ArgumentCollection<HearingAidSimulation> hearingAidSimulation_{};
		ArgumentCollection<Spatialization> spatialization_{};
		ArgumentCollection<float> fullSimulationScale_{};
		ArgumentCollection<float> hearingAidSimulationScale_{};
		ArgumentCollection<float> spatializationScale_{};
		ArgumentCollection<float> withoutSimulationScale_{};
		std::shared_ptr<SignalProcessor> processor{};
	public:
		void setProcessor(std::shared_ptr<SignalProcessor> p) noexcept {
			processor = std::move(p);
		}

		std::shared_ptr<SignalProcessor> make(SimulationParameters p) override {
			parameters_.push_back(std::move(p));
			return processor;
		}

		auto parameters() const {
			return parameters_;
		}

		std::shared_ptr<SignalProcessor> makeFullSimulation(
			FullSimulation s, float x
		) override {
			fullSimulation_.push_back(std::move(s));
			fullSimulationScale_.push_back(x);
			return {};
		}
		std::shared_ptr<SignalProcessor> makeHearingAidSimulation(
			HearingAidSimulation s, float x
		) override {
			hearingAidSimulation_.push_back(std::move(s));
			hearingAidSimulationScale_.push_back(x);
			return {};
		}
		std::shared_ptr<SignalProcessor> makeSpatialization(
			Spatialization s, float x
		) override {
			spatialization_.push_back(std::move(s));
			spatializationScale_.push_back(x);
			return {};
		}
		std::shared_ptr<SignalProcessor> makeWithoutSimulation(
			float x
		) override {
			withoutSimulationScale_.push_back(x);
			return {};
		}
		
		auto fullSimulation() const {
			return fullSimulation_;
		}
		auto hearingAidSimulation() const {
			return hearingAidSimulation_;
		}
		auto spatialization() const {
			return spatialization_;
		}
		auto fullSimulationScale() const {
			return fullSimulationScale_;
		}
		auto hearingAidSimulationScale() const {
			return hearingAidSimulationScale_;
		}
		auto spatializationScale() const {
			return spatializationScale_;
		}
		auto withoutSimulationScale() const {
			return withoutSimulationScale_;
		}
	};

	class RefactoredModelTests : public ::testing::Test {
	protected:
		using channel_type = AudioFrameProcessor::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;

		RefactoredModel::TestParameters testParameters{};
		RefactoredModel::TrialParameters trialParameters{};
		PrescriptionReaderStub prescriptionReader{};
		BrirReaderStub brirReader{};
		SpeechPerceptionTestStub perceptionTest{};
		std::shared_ptr<AudioFrameReaderStub> audioFrameReader
			= std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory audioFrameReaderFactory{ audioFrameReader };
		AudioPlayerStub audioPlayer{};
		AudioLoaderStub audioLoader{};
		SpatializedHearingAidSimulationFactoryStub simulationFactory{};
		RefactoredModel model{
			&perceptionTest,
			&audioPlayer,
			&audioLoader,
			&audioFrameReaderFactory,
			&prescriptionReader,
			&brirReader,
			&simulationFactory
		};

		RefactoredModelTests() {
			setValidDefaults();
		}

		void setValidDefaults() {
			BrirReader::BinauralRoomImpulseResponse brir;
			brir.left = { 0 };
			brir.right = { 0 };
			brirReader.setBrir(brir);
			testParameters.chunkSize = 1;
			testParameters.windowSize = 1;
		}

		void prepareNewTest() {
			model.prepareNewTest(testParameters);
		}

		void playTrial() {
			model.playTrial(trialParameters);
		}

		void setInMemoryReader(AudioFileReader &reader_) {
			audioFrameReaderFactory.setReader(std::make_shared<AudioFileInMemory>(reader_));
		}
	};

	TEST_F(RefactoredModelTests, constructorAssignsAudioLoaderToPlayer) {
		EXPECT_EQ(
			&audioLoader, 
			audioPlayer.audioLoader()
		);
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.leftDslPrescriptionFilePath = "a";
		testParameters.rightDslPrescriptionFilePath = "b";
		prepareNewTest();
		assertTrue(prescriptionReader.filePaths().contains("a"));
		assertTrue(prescriptionReader.filePaths().contains("b"));
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = false;
		prepareNewTest();
		assertFalse(prescriptionReader.readCalled());
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		testParameters.usingSpatialization = true;
		testParameters.brirFilePath = "a";
		prepareNewTest();
		assertEqual("a", brirReader.filePath());
	}

	TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
		testParameters.usingSpatialization = false;
		prepareNewTest();
		assertFalse(brirReader.readCalled());
	}

	TEST_F(RefactoredModelTests, prepareNewTestPassesParametersToSpeechPerceptionTest) {
		testParameters.audioDirectory = "a";
		testParameters.testFilePath = "b";
		testParameters.subjectId = "c";
		testParameters.testerId = "d";
		prepareNewTest();
		assertEqual("a", perceptionTest.testParameters().audioDirectory);
		assertEqual("b", perceptionTest.testParameters().testFilePath);
		assertEqual("c", perceptionTest.testParameters().subjectId);
		assertEqual("d", perceptionTest.testParameters().testerId);
	}

	TEST_F(RefactoredModelTests, playTrialPlaysPlayer) {
		playTrial();
		assertTrue(audioPlayer.played());
	}

	TEST_F(RefactoredModelTests, playTrialPassesNextStimulusToFactory) {
		perceptionTest.setNextStimulus("a");
		playTrial();
		assertEqual("a", audioFrameReaderFactory.filePath());
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		audioPlayer.callOnPlay([&]() {
			EXPECT_EQ(
				audioFrameReader, 
				audioLoader.audioFrameReader()
			);
		});
		playTrial();
	}

	TEST_F(RefactoredModelTests, playTrialPassesReaderMatchedParametersToPlayer) {
		audioFrameReader->setChannels(1);
		audioFrameReader->setSampleRate(2);
		playTrial();
		assertEqual(1, audioPlayer.preparation().channels);
		assertEqual(2, audioPlayer.preparation().sampleRate);
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioDeviceToPlayer) {
		trialParameters.audioDevice = "a";
		playTrial();
		assertEqual("a", audioPlayer.preparation().audioDevice);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.chunkSize = 1;
		prepareNewTest();
		playTrial();
		assertEqual(1, audioPlayer.preparation().framesPerBuffer);
	}

	TEST_F(
		RefactoredModelTests,
		playTrialUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = false;
		prepareNewTest();
		playTrial();
		assertEqual(
			RefactoredModel::defaultFramesPerBuffer, 
			audioPlayer.preparation().framesPerBuffer
		);
	}

	TEST_F(RefactoredModelTests, playTrialComputesCalibrationScalarsForSimulation) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		setInMemoryReader(fakeReader);
		trialParameters.level_dB_Spl = 7;
		playTrial();
		const auto desiredRms = 
			std::pow(10.0, (7 - RefactoredModel::fullScaleLevel_dB_Spl) / 20.0);
		const auto leftChannelRms = std::sqrt((1 * 1 + 3 * 3 + 5 * 5.0) / 3);
		const auto rightChannelRms = std::sqrt((2 * 2 + 4 * 4 + 6 * 6.0) / 3);
		EXPECT_NEAR(desiredRms / leftChannelRms, simulationFactory.parameters().at(0).scale, 1e-6);
		EXPECT_NEAR(desiredRms / rightChannelRms, simulationFactory.parameters().at(1).scale, 1e-6);
	}

	TEST_F(RefactoredModelTests, playTrialResetsReaderAfterComputingRms) {
		playTrial();
		assertTrue(audioFrameReader->readingLog().endsWith("reset "));
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesLeftPrescriptionToFactoryWhenUsingHearingAidSimulation
	) {
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		testParameters.usingHearingAidSimulation = true;
		testParameters.leftDslPrescriptionFilePath = "leftFilePath";
		prescriptionReader.addPrescription("leftFilePath", prescription);
		prepareNewTest();
		playTrial();
		auto actual = simulationFactory.parameters().at(0).prescription;
		assertEqual({ 1 }, actual.compressionRatios);
		assertEqual({ 2 }, actual.crossFrequenciesHz);
		assertEqual({ 3 }, actual.kneepointGains_dB);
		assertEqual({ 4 }, actual.kneepoints_dBSpl);
		assertEqual({ 5 }, actual.broadbandOutputLimitingThresholds_dBSpl);
		assertEqual(6, actual.channels);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesLeftPrescriptionToFactoryForHearingAidSimulation
	) {
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		testParameters.usingHearingAidSimulation = true;
		testParameters.leftDslPrescriptionFilePath = "leftFilePath";
		prescriptionReader.addPrescription("leftFilePath", prescription);
		prepareNewTest();
		playTrial();
		auto actual = simulationFactory.hearingAidSimulation().at(0).prescription;
		assertEqual({ 1 }, actual.compressionRatios);
		assertEqual({ 2 }, actual.crossFrequenciesHz);
		assertEqual({ 3 }, actual.kneepointGains_dB);
		assertEqual({ 4 }, actual.kneepoints_dBSpl);
		assertEqual({ 5 }, actual.broadbandOutputLimitingThresholds_dBSpl);
		assertEqual(6, actual.channels);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesLeftPrescriptionToFactoryForFullSimulation
	) {
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		testParameters.usingHearingAidSimulation = true;
		testParameters.usingSpatialization = true;
		testParameters.leftDslPrescriptionFilePath = "leftFilePath";
		prescriptionReader.addPrescription("leftFilePath", prescription);
		prepareNewTest();
		playTrial();
		auto actual = simulationFactory.fullSimulation().at(0).hearingAid.prescription;
		assertEqual({ 1 }, actual.compressionRatios);
		assertEqual({ 2 }, actual.crossFrequenciesHz);
		assertEqual({ 3 }, actual.kneepointGains_dB);
		assertEqual({ 4 }, actual.kneepoints_dBSpl);
		assertEqual({ 5 }, actual.broadbandOutputLimitingThresholds_dBSpl);
		assertEqual(6, actual.channels);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesRightPrescriptionToFactoryForHearingAidSimulation
	) {
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		testParameters.usingHearingAidSimulation = true;
		testParameters.rightDslPrescriptionFilePath = "rightFilePath";
		prescriptionReader.addPrescription("rightFilePath", prescription);
		prepareNewTest();
		playTrial();
		auto actual = simulationFactory.hearingAidSimulation().at(1).prescription;
		assertEqual({ 1 }, actual.compressionRatios);
		assertEqual({ 2 }, actual.crossFrequenciesHz);
		assertEqual({ 3 }, actual.kneepointGains_dB);
		assertEqual({ 4 }, actual.kneepoints_dBSpl);
		assertEqual({ 5 }, actual.broadbandOutputLimitingThresholds_dBSpl);
		assertEqual(6, actual.channels);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesRightPrescriptionToFactoryForFullSimulation
	) {
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		testParameters.usingHearingAidSimulation = true;
		testParameters.usingSpatialization = true;
		testParameters.rightDslPrescriptionFilePath = "rightFilePath";
		prescriptionReader.addPrescription("rightFilePath", prescription);
		prepareNewTest();
		playTrial();
		auto actual = simulationFactory.fullSimulation().at(1).hearingAid.prescription;
		assertEqual({ 1 }, actual.compressionRatios);
		assertEqual({ 2 }, actual.crossFrequenciesHz);
		assertEqual({ 3 }, actual.kneepointGains_dB);
		assertEqual({ 4 }, actual.kneepoints_dBSpl);
		assertEqual({ 5 }, actual.broadbandOutputLimitingThresholds_dBSpl);
		assertEqual(6, actual.channels);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = false;
		prepareNewTest();
		playTrial();
		assertTrue(simulationFactory.fullSimulation().empty());
		assertTrue(simulationFactory.hearingAidSimulation().empty());
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesRightPrescriptionToFactoryWhenUsingHearingAidSimulation
	) {
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		testParameters.usingHearingAidSimulation = true;
		testParameters.rightDslPrescriptionFilePath = "rightFilePath";
		prescriptionReader.addPrescription("rightFilePath", prescription);
		prepareNewTest();
		playTrial();
		auto actual = simulationFactory.parameters().at(1).prescription;
		assertEqual({ 1 }, actual.compressionRatios);
		assertEqual({ 2 }, actual.crossFrequenciesHz);
		assertEqual({ 3 }, actual.kneepointGains_dB);
		assertEqual({ 4 }, actual.kneepoints_dBSpl);
		assertEqual({ 5 }, actual.broadbandOutputLimitingThresholds_dBSpl);
		assertEqual(6, actual.channels);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesCompressionParametersToFactoryWhenUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.attack_ms = 1;
		testParameters.release_ms = 2;
		testParameters.chunkSize = 4;
		testParameters.windowSize = 8;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.parameters().at(0);
		assertEqual(1.0, left.attack_ms);
		assertEqual(2.0, left.release_ms);
		assertEqual(4, left.chunkSize);
		assertEqual(8, left.windowSize);
		auto right = simulationFactory.parameters().at(1);
		assertEqual(1.0, right.attack_ms);
		assertEqual(2.0, right.release_ms);
		assertEqual(4, right.chunkSize);
		assertEqual(8, right.windowSize);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.attack_ms = 1;
		testParameters.release_ms = 2;
		testParameters.chunkSize = 4;
		testParameters.windowSize = 8;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.hearingAidSimulation().at(0);
		assertEqual(1.0, left.attack_ms);
		assertEqual(2.0, left.release_ms);
		assertEqual(4, left.chunkSize);
		assertEqual(8, left.windowSize);
		auto right = simulationFactory.hearingAidSimulation().at(1);
		assertEqual(1.0, right.attack_ms);
		assertEqual(2.0, right.release_ms);
		assertEqual(4, right.chunkSize);
		assertEqual(8, right.windowSize);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesCompressionParametersToFactoryForFullSimulation
	) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.usingSpatialization = true;
		testParameters.attack_ms = 1;
		testParameters.release_ms = 2;
		testParameters.chunkSize = 4;
		testParameters.windowSize = 8;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.fullSimulation().at(0).hearingAid;
		assertEqual(1.0, left.attack_ms);
		assertEqual(2.0, left.release_ms);
		assertEqual(4, left.chunkSize);
		assertEqual(8, left.windowSize);
		auto right = simulationFactory.fullSimulation().at(1).hearingAid;
		assertEqual(1.0, right.attack_ms);
		assertEqual(2.0, right.release_ms);
		assertEqual(4, right.chunkSize);
		assertEqual(8, right.windowSize);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryWhenUsingHearingAidSimulation
	) {
		audioFrameReader->setSampleRate(1);
		testParameters.usingHearingAidSimulation = true;
		prepareNewTest();
		playTrial();
		assertEqual(1, simulationFactory.parameters().at(0).sampleRate);
		assertEqual(1, simulationFactory.parameters().at(1).sampleRate);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		audioFrameReader->setSampleRate(1);
		testParameters.usingHearingAidSimulation = true;
		prepareNewTest();
		playTrial();
		assertEqual(1, simulationFactory.hearingAidSimulation().at(0).sampleRate);
		assertEqual(1, simulationFactory.hearingAidSimulation().at(1).sampleRate);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		audioFrameReader->setSampleRate(1);
		testParameters.usingHearingAidSimulation = true;
		testParameters.usingSpatialization = true;
		prepareNewTest();
		playTrial();
		assertEqual(1, simulationFactory.fullSimulation().at(0).hearingAid.sampleRate);
		assertEqual(1, simulationFactory.fullSimulation().at(1).hearingAid.sampleRate);
	}

	TEST_F(RefactoredModelTests, playTrialPassesBoolsToFactory) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.usingSpatialization = true;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.parameters().at(0);
		assertTrue(left.usingHearingAidSimulation);
		assertTrue(left.usingSpatialization);
		auto right = simulationFactory.parameters().at(1);
		assertTrue(right.usingHearingAidSimulation);
		assertTrue(right.usingSpatialization);
	}

	TEST_F(RefactoredModelTests, playTrialPassesBoolsToFactory2) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.usingSpatialization = false;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.parameters().at(0);
		assertTrue(left.usingHearingAidSimulation);
		assertFalse(left.usingSpatialization);
		auto right = simulationFactory.parameters().at(1);
		assertTrue(right.usingHearingAidSimulation);
		assertFalse(right.usingSpatialization);
	}

	TEST_F(RefactoredModelTests, playTrialPassesBoolsToFactory3) {
		testParameters.usingHearingAidSimulation = false;
		testParameters.usingSpatialization = true;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.parameters().at(0);
		assertFalse(left.usingHearingAidSimulation);
		assertTrue(left.usingSpatialization);
		auto right = simulationFactory.parameters().at(1);
		assertFalse(right.usingHearingAidSimulation);
		assertTrue(right.usingSpatialization);
	}

	TEST_F(RefactoredModelTests, playTrialPassesBoolsToFactory4) {
		testParameters.usingHearingAidSimulation = false;
		testParameters.usingSpatialization = false;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.parameters().at(0);
		assertFalse(left.usingHearingAidSimulation);
		assertFalse(left.usingSpatialization);
		auto right = simulationFactory.parameters().at(1);
		assertFalse(right.usingHearingAidSimulation);
		assertFalse(right.usingSpatialization);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesFullScaleLevelToFactoryWhenUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = true;
		prepareNewTest();
		playTrial();
		assertEqual(
			RefactoredModel::fullScaleLevel_dB_Spl, 
			simulationFactory.parameters().at(0).fullScaleLevel_dB_Spl
		);
		assertEqual(
			RefactoredModel::fullScaleLevel_dB_Spl, 
			simulationFactory.parameters().at(1).fullScaleLevel_dB_Spl
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesBrirToFactoryWhenUsingSpatialization
	) {
		testParameters.usingSpatialization = true;
		BrirReader::BinauralRoomImpulseResponse brir;
		brir.left = { 1, 2 };
		brir.right = { 3, 4 };
		brirReader.setBrir(brir);
		prepareNewTest();
		playTrial();
		assertEqual({ 1, 2, }, simulationFactory.parameters().at(0).filterCoefficients);
		assertEqual({ 3, 4, }, simulationFactory.parameters().at(1).filterCoefficients);
	}

	TEST_F(RefactoredModelTests, playTrialSetsProcessorBeforePlaying) {
		prepareNewTest();
		simulationFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		buffer_type left{ 4 };
		buffer_type right{ 5 };
		std::vector<channel_type> channels{ left, right };
		audioPlayer.callOnPlay([&]() { audioLoader.audioFrameProcessor()->process(channels); });
		playTrial();
		assertEqual(4 + 1.0f, left.front());
		assertEqual(5 + 1.0f, right.front());
	}

	TEST_F(RefactoredModelTests, playTrialResetsAudioLoaderBeforePlaying) {
		audioPlayer.callOnPlay([&]() { assertTrue(audioLoader.log().contains("reset")); });
		playTrial();
	}

	TEST_F(RefactoredModelTests, playTrialPreparesPlayerBeforePlaying) {
		playTrial();
		assertEqual("prepareToPlay play ", audioPlayer.log());
	}

	TEST_F(RefactoredModelTests, playTrialDoesNotAlterLoaderWhenPlayerPlaying) {
		audioPlayer.setPlaying();
		playTrial();
		assertTrue(audioLoader.log().isEmpty());
	}

	TEST_F(RefactoredModelTests, audioDeviceDescriptionsReturnsDescriptionsFromPlayer) {
		audioPlayer.setAudioDeviceDescriptions({ "a", "b", "c" });
		assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
	}

	TEST_F(RefactoredModelTests, testCompleteWhenTestComplete) {
		perceptionTest.setComplete();
		assertTrue(model.testComplete());
	}

	class RefactoredModelFailureTests : public ::testing::Test {
	protected:
		RefactoredModel::TestParameters testParameters{};
		PrescriptionReaderStub defaultPrescriptionReader{};
		PrescriptionReader *prescriptionReader{ &defaultPrescriptionReader };
		BrirReaderStub defaultBrirReader{};
		BrirReader *brirReader{ &defaultBrirReader };
		SpeechPerceptionTestStub defaultPerceptionTest{};
		SpeechPerceptionTest *perceptionTest{ &defaultPerceptionTest };
		AudioFrameReaderStubFactory defaultAudioReaderFactory{};
		AudioFrameReaderFactory *audioReaderFactory{ &defaultAudioReaderFactory };
		AudioPlayerStub defaultPlayer{};
		IAudioPlayer *audioPlayer{ &defaultPlayer };
		AudioLoaderStub defaultLoader{};
		AudioLoader *audioLoader{ &defaultLoader };
		SpatializedHearingAidSimulationFactoryStub defaultSimulationFactory{};
		ISpatializedHearingAidSimulationFactory *simulationFactory{&defaultSimulationFactory};

		void assertPreparingNewTestThrowsTestInitializationFailure(std::string what) {
			auto model = makeModel();
			try {
				model.prepareNewTest(testParameters);
				FAIL() << "Expected RefactoredModel::TestInitializationFailure.";
			}
			catch (const RefactoredModel::TestInitializationFailure & e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayTrialThrowsTrialFailure(std::string what) {
			auto model = makeModel();
			try {
				model.playTrial({});
				FAIL() << "Expected RefactoredModel::TrialFailure.";
			}
			catch (const RefactoredModel::TrialFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayCalibrationThrowsCalibrationFailure(std::string what) {
			auto model = makeModel();
			try {
				model.playCalibration({});
				FAIL() << "Expected RefactoredModel::CalibrationFailure.";
			}
			catch (const RefactoredModel::CalibrationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void prepareNewTestIgnoringFailure() {
			auto model = makeModel();
			try {
				model.prepareNewTest(testParameters);
			}
			catch (const RefactoredModel::TestInitializationFailure &) {
			}
		}

		void playTrialIgnoringFailure() {
			try {
				auto model = makeModel();
				model.playTrial({});
			}
			catch (const RefactoredModel::TrialFailure &) {
			}
		}

		RefactoredModel makeModel() {
			return
			{
				perceptionTest,
				audioPlayer,
				audioLoader,
				audioReaderFactory,
				prescriptionReader,
				brirReader,
				simulationFactory
			};
		}
	};

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		testParameters.usingHearingAidSimulation = true;
		testParameters.leftDslPrescriptionFilePath = "a";
		assertPreparingNewTestThrowsTestInitializationFailure("Unable to read 'a'.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotPrepareTestWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		testParameters.usingHearingAidSimulation = true;
		prepareNewTestIgnoringFailure();
		assertFalse(defaultPerceptionTest.prepareNewTestCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		testParameters.usingSpatialization = true;
		testParameters.brirFilePath = "a";
		assertPreparingNewTestThrowsTestInitializationFailure("Unable to read 'a'.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotPrepareTestWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		testParameters.usingSpatialization = true;
		prepareNewTestIgnoringFailure();
		assertFalse(defaultPerceptionTest.prepareNewTestCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenPerceptionTestFailsToInitialize
	) {
		InitializationFailingSpeechPerceptionTest failing;
		failing.setErrorMessage("error.");
		perceptionTest = &failing;
		assertPreparingNewTestThrowsTestInitializationFailure("error.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenCoefficientsAreEmpty
	) {
		testParameters.usingSpatialization = true;
		BrirReader::BinauralRoomImpulseResponse brir;
		brir.left = {};
		brir.right = { 0 };
		defaultBrirReader.setBrir(brir);
		assertPreparingNewTestThrowsTestInitializationFailure(
			"The left BRIR coefficients are empty, therefore a filter operation cannot be defined."
		);
		brir.left = { 0 };
		brir.right = {};
		defaultBrirReader.setBrir(brir);
		assertPreparingNewTestThrowsTestInitializationFailure(
			"The right BRIR coefficients are empty, therefore a filter operation cannot be defined."
		);
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenWindowOrChunkSizeIsNotPowerOfTwo
	) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.chunkSize = 0;
		testParameters.windowSize = 1;
		assertPreparingNewTestThrowsTestInitializationFailure(
			"Both the chunk size and window size must be powers of two; 0 is not a power of two."
		);
		testParameters.chunkSize = 2;
		testParameters.windowSize = 3;
		assertPreparingNewTestThrowsTestInitializationFailure(
			"Both the chunk size and window size must be powers of two; 3 is not a power of two."
		);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playTrialThrowsTrialFailureWhenAudioFrameReaderCannotBeCreated
	) {
		ErrorAudioFrameReaderFactory failing{ "error." };
		audioReaderFactory = &failing;
		assertPlayTrialThrowsTrialFailure("error.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playTrialThrowsTrialFailureWhenPlayerThrowsRequestFailure
	) {
		PreparationFailingAudioPlayer failing;
		failing.setErrorMessage("error.");
		audioPlayer = &failing;
		assertPlayTrialThrowsTrialFailure("error.");
	}

	TEST_F(RefactoredModelFailureTests, playTrialDoesNotAdvancePerceptionTestTrialWhenPlayerFails) {
		PreparationFailingAudioPlayer failing;
		audioPlayer = &failing;
		playTrialIgnoringFailure();
		assertFalse(defaultPerceptionTest.advanceTrialCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsCalibrationFailureWhenPlayerThrowsPreparationFailure
	) {
		PreparationFailingAudioPlayer failing;
		failing.setErrorMessage("error.");
		audioPlayer = &failing;
		assertPlayCalibrationThrowsCalibrationFailure("error.");
	}
}