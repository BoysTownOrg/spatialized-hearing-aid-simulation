#include "ArgumentCollection.h"
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>

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

class AudioPlayerStub : public AudioStimulusPlayer {
	std::vector<std::string> audioDeviceDescriptions_{};
	Preparation preparation_{};
	AudioLoader *audioLoader_{};
public:
	void prepareToPlay(Preparation p) override {
		preparation_ = std::move(p);
	}

	const Preparation &preparation() const noexcept {
		return preparation_;
	}
	
	std::vector<std::string> audioDeviceDescriptions() override {
		return audioDeviceDescriptions_;
	}

	void setAudioDeviceDescriptions(std::vector<std::string> v) {
		audioDeviceDescriptions_ = std::move(v);
	}

	void play() override
	{
	}

	bool isPlaying() override
	{
		return false;
	}
	
	void stop() override
	{
	}

	void setAudioLoader(AudioLoader *a) override {
		audioLoader_ = a;
	}

	const AudioLoader *audioLoader() const noexcept {
		return audioLoader_;
	}
};

class PreparationFailingAudioPlayer : public AudioStimulusPlayer {
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
	void stop() override {}
	void setAudioLoader(AudioLoader *) override {}
};

class SpeechPerceptionTestStub : public SpeechPerceptionTest {
	TestParameters testParameters_{};
	std::string nextStimulus_{};
	StimulusPlayer *player_{};
	bool playNextTrialCalled_{};
public:
	const auto &testParameters() const noexcept {
		return testParameters_;
	}

	void prepareNewTest(TestParameters p) override {
		testParameters_ = std::move(p);
	}

	void playNextTrial(StimulusPlayer *p) override {
		player_ = p;
		playNextTrialCalled_ = true;
	}
	
	bool playNextTrialCalled() const noexcept {
		return playNextTrialCalled_;
	}

	void setNextStimulus(std::string s) {
		nextStimulus_ = std::move(s);
	}

	std::string nextStimulus() override {
		return nextStimulus_;
	}

	const auto player() const noexcept {
		return player_;
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

	void playNextTrial(StimulusPlayer *) override {}
	std::string nextStimulus() override { return {}; }
};

#include "assert-utility.h"
#include "FilterbankCompressorSpy.h"
#include "AudioFrameReaderStub.h"
#include "AudioLoaderStub.h"
#include "PrescriptionReaderStub.h"
#include "BrirReaderStub.h"
#include "FakeAudioFileReader.h"
#include "SignalProcessorStub.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

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
	AudioFrameReaderStubFactory audioFrameReaderFactory{audioFrameReader};
	AudioPlayerStub audioPlayer{};
	AudioLoaderStub audioLoader{};
	RefactoredModel model{ 
		&perceptionTest,
		&audioPlayer,
		&audioLoader,
		&audioFrameReaderFactory,
		&hearingAidFactory, 
		&prescriptionReader, 
		&firFilterFactory,
		&brirReader, 
		&scalarFactory
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
	prepareNewTest();
	assertEqual("a", perceptionTest.testParameters().audioDirectory);
	assertEqual("b", perceptionTest.testParameters().testFilePath);
}

TEST_F(RefactoredModelTests, playTrialPassesStimulusPlayerToSpeechPerceptionTest) {
	playTrial();
	EXPECT_EQ(&audioPlayer, perceptionTest.player());
}

TEST_F(RefactoredModelTests, playTrialPassesNextStimulusToFactory) {
	perceptionTest.setNextStimulus("a");
	playTrial();
	assertEqual("a", audioFrameReaderFactory.filePath());
}

TEST_F(RefactoredModelTests, playTrialPassesAudioFrameReaderToAudioLoader) {
	playTrial();
	EXPECT_EQ(audioFrameReader, audioLoader.audioFrameReader());
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
	EXPECT_NEAR(desiredRms / leftChannelRms, scalarFactory.scalars().at(0), 1e-6);
	EXPECT_NEAR(desiredRms / rightChannelRms, scalarFactory.scalars().at(1), 1e-6);
}

TEST_F(RefactoredModelTests, playTrialPassesLeftPrescriptionToHearingAidFactory) {
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
	auto left = hearingAidFactory.parameters().at(0);
	assertEqual({ 1 }, left.compressionRatios);
	assertEqual({ 2 }, left.crossFrequenciesHz);
	assertEqual({ 3 }, left.kneepointGains_dB);
	assertEqual({ 4 }, left.kneepoints_dBSpl);
	assertEqual({ 5 }, left.broadbandOutputLimitingThresholds_dBSpl);
	EXPECT_EQ(6, left.channels);
}

TEST_F(RefactoredModelTests, playTrialPassesRightPrescriptionToHearingAidFactory) {
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
	auto right = hearingAidFactory.parameters().at(1);
	assertEqual({ 1 }, right.compressionRatios);
	assertEqual({ 2 }, right.crossFrequenciesHz);
	assertEqual({ 3 }, right.kneepointGains_dB);
	assertEqual({ 4 }, right.kneepoints_dBSpl);
	assertEqual({ 5 }, right.broadbandOutputLimitingThresholds_dBSpl);
	EXPECT_EQ(6, right.channels);
}

TEST_F(RefactoredModelTests, playTrialPassesOtherCompressionParametersToHearingAidFactory) {
	testParameters.usingHearingAidSimulation = true;
	testParameters.attack_ms = 1;
	testParameters.release_ms = 2;
	testParameters.chunkSize = 4;
	testParameters.windowSize = 8;
	prepareNewTest();
	playTrial();
	auto left = hearingAidFactory.parameters().at(0);
	EXPECT_EQ(1, left.attack_ms);
	EXPECT_EQ(2, left.release_ms);
	EXPECT_EQ(4, left.chunkSize);
	EXPECT_EQ(8, left.windowSize);
	auto right = hearingAidFactory.parameters().at(1);
	EXPECT_EQ(1, right.attack_ms);
	EXPECT_EQ(2, right.release_ms);
	EXPECT_EQ(4, right.chunkSize);
	EXPECT_EQ(8, right.windowSize);
}

TEST_F(RefactoredModelTests, playTrialPassesSampleRateFromAudioReaderToHearingAidFactory) {
	audioFrameReader->setSampleRate(1);
	testParameters.usingHearingAidSimulation = true;
	prepareNewTest();
	playTrial();
	auto left = hearingAidFactory.parameters().at(0);
	EXPECT_EQ(1, left.sampleRate);
	auto right = hearingAidFactory.parameters().at(1);
	EXPECT_EQ(1, right.sampleRate);
}

TEST_F(RefactoredModelTests, playTrialPassesBrirToFirFilterFactory) {
	BrirReader::BinauralRoomImpulseResponse brir;
	brir.left = { 1, 2 };
	brir.right = { 3, 4 };
	brirReader.setBrir(brir);
	testParameters.usingSpatialization = true;
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
	firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(3.0f));
	hearingAidFactory.setProcessor(std::make_shared <AddsSamplesBy>(2.0f));
	playTrial();
	auto processor = audioLoader.audioFrameProcessor();
	std::vector<float> left{ 4 };
	std::vector<float> right{ 5 };
	std::vector<gsl::span<float>> channels{ left, right };
	processor->process(channels);
	EXPECT_EQ((4 + 1) * 3 + 2, left.at(0));
	EXPECT_EQ((5 + 1) * 3 + 2, right.at(0));
}

TEST_F(RefactoredModelTests, playTrialNoSpatialization) {
	testParameters.usingSpatialization = false;
	testParameters.usingHearingAidSimulation = true;
	audioFrameReader->setChannels(2);
	prepareNewTest();
	scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
	firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(3.0f));
	hearingAidFactory.setProcessor(std::make_shared <AddsSamplesBy>(2.0f));
	playTrial();
	auto processor = audioLoader.audioFrameProcessor();
	std::vector<float> left{ 4 };
	std::vector<float> right{ 5 };
	std::vector<gsl::span<float>> channels{ left, right };
	processor->process(channels);
	EXPECT_EQ(4 + 1 + 2, left.at(0));
	EXPECT_EQ(5 + 1 + 2, right.at(0));
}

TEST_F(RefactoredModelTests, playTrialNoHearingAidSimulation) {
	testParameters.usingSpatialization = true;
	testParameters.usingHearingAidSimulation = false;
	audioFrameReader->setChannels(2);
	prepareNewTest();
	scalarFactory.setProcessor(std::make_shared<AddsSamplesBy>(1.0f));
	firFilterFactory.setProcessor(std::make_shared<MultipliesSamplesBy>(3.0f));
	hearingAidFactory.setProcessor(std::make_shared <AddsSamplesBy>(2.0f));
	playTrial();
	auto processor = audioLoader.audioFrameProcessor();
	std::vector<float> left{ 4 };
	std::vector<float> right{ 5 };
	std::vector<gsl::span<float>> channels{ left, right };
	processor->process(channels);
	EXPECT_EQ((4 + 1) * 3, left.at(0));
	EXPECT_EQ((5 + 1) * 3, right.at(0));
}

TEST_F(RefactoredModelTests, audioDeviceDescriptionsReturnsDescriptionsFromPlayer) {
	audioPlayer.setAudioDeviceDescriptions({ "a", "b", "c" });
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
}

TEST_F(RefactoredModelTests, playTrialResetsAudioLoader) {
	playTrial();
	EXPECT_TRUE(audioLoader.log().contains("reset"));
}

class RefactoredModelFailureTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters newTest{};
	PrescriptionReaderStub defaultPrescriptionReader{};
	PrescriptionReader *prescriptionReader{&defaultPrescriptionReader};
	BrirReaderStub defaultBrirReader{};
	BrirReader *brirReader{&defaultBrirReader};
	SpeechPerceptionTestStub defaultTest{};
	SpeechPerceptionTest *test{&defaultTest};
	HearingAidFactoryStub defaultHearingAidFactory{};
	HearingAidFactory *hearingAidFactory{&defaultHearingAidFactory};
	FirFilterFactoryStub defaultFirFilterFactory{};
	FirFilterFactory *firFilterFactory{ &defaultFirFilterFactory };
	ScalarFactoryStub defaultScalarFactory{};
	ScalarFactory *scalarFactory{ &defaultScalarFactory };
	AudioFrameReaderStubFactory defaultAudioReaderFactory{};
	AudioFrameReaderFactory *audioReaderFactory{&defaultAudioReaderFactory};
	AudioPlayerStub defaultPlayer{};
	AudioStimulusPlayer *player{ &defaultPlayer };
	AudioLoaderStub defaultLoader{};
	AudioLoader *loader{ &defaultLoader };

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
			scalarFactory
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
	playTrialThrowsTrialFailureWhenPlayerThrowsRequestFailure
) {
	PreparationFailingAudioPlayer failing;
	failing.setErrorMessage("error.");
	player = &failing;
	assertPlayTrialThrowsTrialFailure("error.");
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

TEST_F(RefactoredModelFailureTests, playTrialDoesNotPlayTrialWhenPlayerFails) {
	PreparationFailingAudioPlayer failing;
	player = &failing;
	auto model = makeModel();
	try {
		model.playTrial({});
	}
	catch (const RefactoredModel::TrialFailure &) {
	}
	EXPECT_FALSE(defaultTest.playNextTrialCalled());
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