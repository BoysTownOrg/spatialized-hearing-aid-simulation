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
#include <audio-file-reading/AudioFileInMemory.h>
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>
#include <gtest/gtest.h>

namespace {
	class FirFilterFactoryStub : public FirFilterFactory {
		ArgumentCollection<BrirReader::impulse_response_type> coefficients_{};
		std::shared_ptr<SignalProcessor> processor{};
	public:
		void setProcessor(std::shared_ptr<SignalProcessor> p) noexcept {
			processor = std::move(p);
		}

		auto coefficients() const {
			return coefficients_;
		}

		std::shared_ptr<SignalProcessor> make(BrirReader::impulse_response_type b) override {
			coefficients_.push_back(std::move(b));
			return processor;
		}
	};

	class HearingAidFactoryStub : public HearingAidFactory {
		ArgumentCollection<FilterbankCompressor::Parameters> parameters_{};
		std::shared_ptr<SignalProcessor> processor{};
	public:
		void setProcessor(std::shared_ptr<SignalProcessor> p) noexcept {
			processor = std::move(p);
		}

		auto parameters() const {
			return parameters_;
		}

		std::shared_ptr<SignalProcessor> make(FilterbankCompressor::Parameters p) override {
			parameters_.push_back(std::move(p));
			return processor;
		}
	};

	class ScalarFactoryStub : public ScalarFactory {
		ArgumentCollection<float> scalars_{};
		std::shared_ptr<SignalProcessor> processor{};
	public:
		void setProcessor(std::shared_ptr<SignalProcessor> p) noexcept {
			processor = std::move(p);
		}

		auto scalars() const {
			return scalars_;
		}

		std::shared_ptr<SignalProcessor> make(float x) override {
			scalars_.push_back(x);
			return processor;
		}
	};

	class RefactoredSpatializedHearingAidSimulationFactoryStub : 
		public IRefactoredSpatializedHearingAidSimulationFactory 
	{
		ArgumentCollection<SimulationParameters> parameters_{};
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
	};

	class AudioPlayerStub : public IAudioPlayer {
		std::vector<std::string> audioDeviceDescriptions_{};
		Preparation preparation_{};
		LogString log_{};
		std::function<void(void)> callOnPlay_{ []() {} };
		AudioLoader *audioLoader_{};
		bool isPlaying_{};
		bool played_{};
	public:
		void prepareToPlay(Preparation p) override {
			preparation_ = std::move(p);
			log_ += LogString{ "prepareToPlay " };
		}

		const auto &preparation() const noexcept {
			return preparation_;
		}

		std::vector<std::string> audioDeviceDescriptions() override {
			return audioDeviceDescriptions_;
		}

		void setAudioDeviceDescriptions(std::vector<std::string> v) {
			audioDeviceDescriptions_ = std::move(v);
		}

		auto played() const {
			return played_;
		}

		void play() override {
			played_ = true;
			callOnPlay_();
			log_ += LogString{ "play " };
		}

		void setPlaying() {
			isPlaying_ = true;
		}

		bool isPlaying() override {
			return isPlaying_;
		}

		void setAudioLoader(AudioLoader *a) override {
			audioLoader_ = a;
		}

		auto audioLoader() const noexcept {
			return audioLoader_;
		}

		void callOnPlay(std::function<void(void)> f) {
			callOnPlay_ = f;
		}

		auto log() const {
			return log_;
		}
	};

	class PreparationFailingAudioPlayer : public IAudioPlayer {
		std::string errorMessage{};
	public:
		void setErrorMessage(std::string s) {
			errorMessage = std::move(s);
		}

		void prepareToPlay(Preparation) override {
			throw PreparationFailure{ errorMessage };
		}

		std::vector<std::string> audioDeviceDescriptions() override { return {}; }
		void play() override {}
		bool isPlaying() override { return {}; }
		void setAudioLoader(AudioLoader *) override {}
	};

	class SpeechPerceptionTestStub : public SpeechPerceptionTest {
		TestParameters testParameters_{};
		std::string nextStimulus_{};
		bool advanceTrialCalled_{};
		bool prepareNewTestCalled_{};
		bool complete_{};
	public:
		const auto &testParameters() const noexcept {
			return testParameters_;
		}

		void prepareNewTest(TestParameters p) override {
			testParameters_ = std::move(p);
			prepareNewTestCalled_ = true;
		}

		auto prepareNewTestCalled() const {
			return prepareNewTestCalled_;
		}

		void advanceTrial() override {
			advanceTrialCalled_ = true;
		}

		auto advanceTrialCalled() const noexcept {
			return advanceTrialCalled_;
		}

		void setNextStimulus(std::string s) {
			nextStimulus_ = std::move(s);
		}

		std::string nextStimulus() override {
			return nextStimulus_;
		}

		void setComplete() {
			complete_ = true;
		}

		bool testComplete() override {
			return complete_;
		}
	};

	class InitializationFailingSpeechPerceptionTest : public SpeechPerceptionTest {
		std::string errorMessage{};
	public:
		void setErrorMessage(std::string s) {
			errorMessage = std::move(s);
		}

		void prepareNewTest(TestParameters) override {
			throw TestInitializationFailure{ errorMessage };
		}

		void advanceTrial() override {}
		std::string nextStimulus() override { return {}; }
		bool testComplete() override { return {}; }
	};

	class RefactoredModelTests : public ::testing::Test {
	protected:
		RefactoredModel::TestParameters testParameters{};
		RefactoredModel::TrialParameters trialParameters{};
		PrescriptionReaderStub prescriptionReader{};
		BrirReaderStub brirReader{};
		SpeechPerceptionTestStub perceptionTest{};
		HearingAidFactoryStub hearingAidFactory{};
		FirFilterFactoryStub firFilterFactory{};
		ScalarFactoryStub scalarFactory{};
		std::shared_ptr<AudioFrameReaderStub> audioFrameReader
			= std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory audioFrameReaderFactory{ audioFrameReader };
		AudioPlayerStub audioPlayer{};
		AudioLoaderStub audioLoader{};
		RefactoredSpatializedHearingAidSimulationFactoryStub simulationFactory{};
		RefactoredModel model{
			&perceptionTest,
			&audioPlayer,
			&audioLoader,
			&audioFrameReaderFactory,
			&hearingAidFactory,
			&prescriptionReader,
			&firFilterFactory,
			&brirReader,
			&scalarFactory,
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
		EXPECT_EQ(&audioLoader, audioPlayer.audioLoader());
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.leftDslPrescriptionFilePath = "a";
		testParameters.rightDslPrescriptionFilePath = "b";
		prepareNewTest();
		EXPECT_TRUE(prescriptionReader.filePaths().contains("a"));
		EXPECT_TRUE(prescriptionReader.filePaths().contains("b"));
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		testParameters.usingHearingAidSimulation = false;
		prepareNewTest();
		EXPECT_FALSE(prescriptionReader.readCalled());
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
		EXPECT_FALSE(brirReader.readCalled());
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
		EXPECT_TRUE(audioPlayer.played());
	}

	TEST_F(RefactoredModelTests, playTrialPassesNextStimulusToFactory) {
		perceptionTest.setNextStimulus("a");
		playTrial();
		assertEqual("a", audioFrameReaderFactory.filePath());
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		audioPlayer.callOnPlay([&]() {
			EXPECT_EQ(audioFrameReader, audioLoader.audioFrameReader());
		});
		playTrial();
	}

	TEST_F(RefactoredModelTests, playTrialPassesReaderMatchedParametersToPlayer) {
		audioFrameReader->setChannels(1);
		audioFrameReader->setSampleRate(2);
		playTrial();
		EXPECT_EQ(1, audioPlayer.preparation().channels);
		EXPECT_EQ(2, audioPlayer.preparation().sampleRate);
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioDeviceToPlayer) {
		trialParameters.audioDevice = "a";
		playTrial();
		assertEqual("a", audioPlayer.preparation().audioDevice);
	}

	TEST_F(RefactoredModelTests, playTrialUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation) {
		testParameters.usingHearingAidSimulation = true;
		testParameters.chunkSize = 1;
		prepareNewTest();
		playTrial();
		EXPECT_EQ(1, audioPlayer.preparation().framesPerBuffer);
	}

	TEST_F(RefactoredModelTests, playTrialResetsReaderAfterComputingRms) {
		playTrial();
		EXPECT_TRUE(audioFrameReader->readingLog().endsWith("reset "));
	}

	TEST_F(RefactoredModelTests, playTrialPassesCalibrationScalarsToFactory) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		setInMemoryReader(fakeReader);
		trialParameters.level_dB_Spl = 7;
		playTrial();
		const auto desiredRms = std::pow(10.0, (7 - RefactoredModel::fullScaleLevel_dB_Spl) / 20.0);
		const auto leftChannelRms = std::sqrt((1.0 * 1.0 + 3.0 * 3.0 + 5.0 * 5.0) / 3);
		const auto rightChannelRms = std::sqrt((2.0 * 2.0 + 4.0 * 4.0 + 6.0 * 6.0) / 3);
		EXPECT_NEAR(desiredRms / leftChannelRms, simulationFactory.parameters().at(0).scale, 1e-6);
		EXPECT_NEAR(desiredRms / rightChannelRms, simulationFactory.parameters().at(1).scale, 1e-6);
	}

	TEST_F(RefactoredModelTests, playTrialPassesLeftPrescriptionToFactory) {
		audioFrameReader->setChannels(1);
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		prescriptionReader.addPrescription("leftFilePath", prescription);
		testParameters.usingHearingAidSimulation = true;
		testParameters.leftDslPrescriptionFilePath = "leftFilePath";
		prepareNewTest();
		playTrial();
		auto actual = simulationFactory.parameters().at(0).prescription;
		assertEqual({ 1 }, actual.compressionRatios);
		assertEqual({ 2 }, actual.crossFrequenciesHz);
		assertEqual({ 3 }, actual.kneepointGains_dB);
		assertEqual({ 4 }, actual.kneepoints_dBSpl);
		assertEqual({ 5 }, actual.broadbandOutputLimitingThresholds_dBSpl);
		EXPECT_EQ(6, actual.channels);
	}

	TEST_F(RefactoredModelTests, playTrialPassesRightPrescriptionToFactory) {
		audioFrameReader->setChannels(2);
		PrescriptionReader::Dsl prescription;
		prescription.compressionRatios = { 1 };
		prescription.crossFrequenciesHz = { 2 };
		prescription.kneepointGains_dB = { 3 };
		prescription.kneepoints_dBSpl = { 4 };
		prescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
		prescription.channels = 6;
		prescriptionReader.addPrescription("rightFilePath", prescription);
		testParameters.usingHearingAidSimulation = true;
		testParameters.rightDslPrescriptionFilePath = "rightFilePath";
		prepareNewTest();
		playTrial();
		auto actual = simulationFactory.parameters().at(1).prescription;
		assertEqual({ 1 }, actual.compressionRatios);
		assertEqual({ 2 }, actual.crossFrequenciesHz);
		assertEqual({ 3 }, actual.kneepointGains_dB);
		assertEqual({ 4 }, actual.kneepoints_dBSpl);
		assertEqual({ 5 }, actual.broadbandOutputLimitingThresholds_dBSpl);
		EXPECT_EQ(6, actual.channels);
	}

	TEST_F(RefactoredModelTests, playTrialPassesOtherCompressionParametersToFactory) {
		audioFrameReader->setChannels(2);
		testParameters.usingHearingAidSimulation = true;
		testParameters.attack_ms = 1;
		testParameters.release_ms = 2;
		testParameters.chunkSize = 4;
		testParameters.windowSize = 8;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.parameters().at(0);
		EXPECT_EQ(1, left.attack_ms);
		EXPECT_EQ(2, left.release_ms);
		EXPECT_EQ(4, left.chunkSize);
		EXPECT_EQ(8, left.windowSize);
		auto right = simulationFactory.parameters().at(1);
		EXPECT_EQ(1, right.attack_ms);
		EXPECT_EQ(2, right.release_ms);
		EXPECT_EQ(4, right.chunkSize);
		EXPECT_EQ(8, right.windowSize);
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioReaderSampleRateToFactory) {
		audioFrameReader->setChannels(2);
		audioFrameReader->setSampleRate(1);
		testParameters.usingHearingAidSimulation = true;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.parameters().at(0);
		EXPECT_EQ(1, left.sampleRate);
		auto right = simulationFactory.parameters().at(1);
		EXPECT_EQ(1, right.sampleRate);
	}

	TEST_F(RefactoredModelTests, playTrialPassesFullScaleLevelToFactory) {
		audioFrameReader->setChannels(2);
		testParameters.usingHearingAidSimulation = true;
		prepareNewTest();
		playTrial();
		auto left = simulationFactory.parameters().at(0);
		EXPECT_EQ(RefactoredModel::fullScaleLevel_dB_Spl, left.fullScaleLevel_dB_Spl);
		auto right = simulationFactory.parameters().at(1);
		EXPECT_EQ(RefactoredModel::fullScaleLevel_dB_Spl, right.fullScaleLevel_dB_Spl);
	}

	TEST_F(RefactoredModelTests, playTrialPassesBrirToFirFilterFactory) {
		testParameters.usingSpatialization = true;
		BrirReader::BinauralRoomImpulseResponse brir;
		brir.left = { 1, 2 };
		brir.right = { 3, 4 };
		brirReader.setBrir(brir);
		prepareNewTest();
		playTrial();
		EXPECT_TRUE(firFilterFactory.coefficients().contains({ 1, 2 }));
		EXPECT_TRUE(firFilterFactory.coefficients().contains({ 3, 4 }));
	}

	TEST_F(RefactoredModelTests, playTrialLoadsLoaderWithProcessor) {
		testParameters.usingSpatialization = true;
		testParameters.usingHearingAidSimulation = true;
		audioFrameReader->setChannels(2);
		prepareNewTest();
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared <AddsSamplesBy>(3.0f));
		playTrial();
		auto processor = audioLoader.audioFrameProcessor();
		std::vector<float> left{ 4 };
		std::vector<float> right{ 5 };
		std::vector<gsl::span<float>> channels{ left, right };
		processor->process(channels);
		EXPECT_EQ((4 + 1) * 2 + 3, left.at(0));
		EXPECT_EQ((5 + 1) * 2 + 3, right.at(0));
	}

	TEST_F(RefactoredModelTests, playTrialNoSpatialization) {
		testParameters.usingSpatialization = false;
		testParameters.usingHearingAidSimulation = true;
		audioFrameReader->setChannels(2);
		prepareNewTest();
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared <AddsSamplesBy>(3.0f));
		playTrial();
		auto processor = audioLoader.audioFrameProcessor();
		std::vector<float> left{ 4 };
		std::vector<float> right{ 5 };
		std::vector<gsl::span<float>> channels{ left, right };
		processor->process(channels);
		EXPECT_EQ(4 + 1 + 3, left.at(0));
		EXPECT_EQ(5 + 1 + 3, right.at(0));
	}

	TEST_F(RefactoredModelTests, playTrialNoHearingAidSimulation) {
		testParameters.usingSpatialization = true;
		testParameters.usingHearingAidSimulation = false;
		audioFrameReader->setChannels(2);
		prepareNewTest();
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared <AddsSamplesBy>(3.0f));
		playTrial();
		auto processor = audioLoader.audioFrameProcessor();
		std::vector<float> left{ 4 };
		std::vector<float> right{ 5 };
		std::vector<gsl::span<float>> channels{ left, right };
		processor->process(channels);
		EXPECT_EQ((4 + 1) * 2, left.at(0));
		EXPECT_EQ((5 + 1) * 2, right.at(0));
	}

	TEST_F(RefactoredModelTests, playTrialLoadsLoaderWithProcessorBeforePlayingPlayer) {
		testParameters.usingSpatialization = true;
		testParameters.usingHearingAidSimulation = true;
		audioFrameReader->setChannels(2);
		prepareNewTest();
		scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
		firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(2.0f));
		hearingAidFactory.setProcessor(std::make_shared <AddsSamplesBy>(3.0f));
		std::vector<float> left{ 4 };
		std::vector<float> right{ 5 };
		std::vector<gsl::span<float>> channels{ left, right };
		audioPlayer.callOnPlay([&]() { audioLoader.audioFrameProcessor()->process(channels); });
		playTrial();
		EXPECT_EQ((4 + 1) * 2 + 3, left.at(0));
		EXPECT_EQ((5 + 1) * 2 + 3, right.at(0));
	}

	TEST_F(RefactoredModelTests, audioDeviceDescriptionsReturnsDescriptionsFromPlayer) {
		audioPlayer.setAudioDeviceDescriptions({ "a", "b", "c" });
		assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
	}

	TEST_F(RefactoredModelTests, playTrialResetsAudioLoaderBeforePlayingNextTrial) {
		audioPlayer.callOnPlay([&]() { EXPECT_TRUE(audioLoader.log().contains("reset")); });
		playTrial();
	}

	TEST_F(RefactoredModelTests, playTrialDoesNotAlterLoaderWhenPlayerPlaying) {
		audioPlayer.setPlaying();
		playTrial();
		EXPECT_TRUE(audioLoader.log().isEmpty());
	}

	TEST_F(RefactoredModelTests, playTrialPreparesPlayerBeforePlaying) {
		playTrial();
		assertEqual("prepareToPlay play ", audioPlayer.log());
	}

	TEST_F(RefactoredModelTests, testCompleteWhenTestComplete) {
		perceptionTest.setComplete();
		EXPECT_TRUE(model.testComplete());
	}

	class RefactoredModelFailureTests : public ::testing::Test {
	protected:
		RefactoredModel::TestParameters newTest{};
		PrescriptionReaderStub defaultPrescriptionReader{};
		PrescriptionReader *prescriptionReader{ &defaultPrescriptionReader };
		BrirReaderStub defaultBrirReader{};
		BrirReader *brirReader{ &defaultBrirReader };
		SpeechPerceptionTestStub defaultTest{};
		SpeechPerceptionTest *test{ &defaultTest };
		HearingAidFactoryStub defaultHearingAidFactory{};
		HearingAidFactory *hearingAidFactory{ &defaultHearingAidFactory };
		FirFilterFactoryStub defaultFirFilterFactory{};
		FirFilterFactory *firFilterFactory{ &defaultFirFilterFactory };
		ScalarFactoryStub defaultScalarFactory{};
		ScalarFactory *scalarFactory{ &defaultScalarFactory };
		AudioFrameReaderStubFactory defaultAudioReaderFactory{};
		AudioFrameReaderFactory *audioReaderFactory{ &defaultAudioReaderFactory };
		AudioPlayerStub defaultPlayer{};
		IAudioPlayer *player{ &defaultPlayer };
		AudioLoaderStub defaultLoader{};
		AudioLoader *loader{ &defaultLoader };
		RefactoredSpatializedHearingAidSimulationFactoryStub defaultSimulationFactory{};
		IRefactoredSpatializedHearingAidSimulationFactory *simulationFactory{&defaultSimulationFactory};

		void assertPreparingNewTestThrowsTestInitializationFailure(std::string what) {
			auto model = makeModel();
			try {
				model.prepareNewTest(newTest);
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

		RefactoredModel makeModel() {
			return
			{
				test,
				player,
				loader,
				audioReaderFactory,
				hearingAidFactory,
				prescriptionReader,
				firFilterFactory,
				brirReader,
				scalarFactory,
				simulationFactory
			};
		}
	};

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		failing.setErrorMessage("irrelevant");
		newTest.usingHearingAidSimulation = true;
		newTest.leftDslPrescriptionFilePath = "a";
		prescriptionReader = &failing;
		assertPreparingNewTestThrowsTestInitializationFailure("Unable to read 'a'.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotPrepareTestWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		newTest.usingHearingAidSimulation = true;
		prescriptionReader = &failing;
		auto model = makeModel();
		try {
			model.prepareNewTest(newTest);
		}
		catch (const RefactoredModel::TestInitializationFailure &) {
		}
		EXPECT_FALSE(defaultTest.prepareNewTestCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenTestFailsToInitialize
	) {
		InitializationFailingSpeechPerceptionTest failing;
		failing.setErrorMessage("error.");
		test = &failing;
		assertPreparingNewTestThrowsTestInitializationFailure("error.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		failing.setErrorMessage("irrelevant.");
		brirReader = &failing;
		newTest.usingSpatialization = true;
		newTest.brirFilePath = "a";
		assertPreparingNewTestThrowsTestInitializationFailure("Unable to read 'a'.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotPrepareTestWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		newTest.usingSpatialization = true;
		auto model = makeModel();
		try {
			model.prepareNewTest(newTest);
		}
		catch (const RefactoredModel::TestInitializationFailure &) {
		}
		EXPECT_FALSE(defaultTest.prepareNewTestCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		playTrialThrowsTrialFailureWhenPlayerThrowsRequestFailure
	) {
		PreparationFailingAudioPlayer failing;
		failing.setErrorMessage("error.");
		player = &failing;
		assertPlayTrialThrowsTrialFailure("error.");
	}

	TEST_F(RefactoredModelFailureTests, playTrialDoesNotAdvanceTrialWhenPlayerFails) {
		PreparationFailingAudioPlayer failing;
		player = &failing;
		auto model = makeModel();
		try {
			model.playTrial({});
		}
		catch (const RefactoredModel::TrialFailure &) {
		}
		EXPECT_FALSE(defaultTest.advanceTrialCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsCalibrationFailureWhenPlayerThrowsRequestFailure
	) {
		PreparationFailingAudioPlayer failing;
		failing.setErrorMessage("error.");
		player = &failing;
		assertPlayCalibrationThrowsCalibrationFailure("error.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsTestInitializationFailureWhenCoefficientsAreEmpty
	) {
		BrirReader::BinauralRoomImpulseResponse brir;
		brir.left = {};
		brir.right = { 0 };
		defaultBrirReader.setBrir(brir);
		newTest.usingSpatialization = true;
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
		newTest.chunkSize = 0;
		newTest.windowSize = 1;
		newTest.usingHearingAidSimulation = true;
		assertPreparingNewTestThrowsTestInitializationFailure(
			"Both the chunk size and window size must be powers of two; 0 is not a power of two."
		);
		newTest.chunkSize = 2;
		newTest.windowSize = 3;
		assertPreparingNewTestThrowsTestInitializationFailure(
			"Both the chunk size and window size must be powers of two; 3 is not a power of two."
		);
	}
}