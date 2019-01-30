#include "ArgumentCollection.h"
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>

class FirFilterFactoryStub : public FirFilterFactory {
	ArgumentCollection<BrirReader::impulse_response_type> coefficients_{};
public:
	auto coefficients() const {
		return coefficients_;
	}

	std::shared_ptr<SignalProcessor> make(BrirReader::impulse_response_type b) override {
		coefficients_.push_back(std::move(b));
		return {};
	}
};

class HearingAidFactoryStub : public HearingAidFactory {
	ArgumentCollection<FilterbankCompressor::Parameters> parameters_{};
public:
	ArgumentCollection<FilterbankCompressor::Parameters> parameters() const {
		return parameters_;
	}

	std::shared_ptr<SignalProcessor> make(FilterbankCompressor::Parameters p) override {
		parameters_.push_back(std::move(p));
		return {};
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

	const Preparation &preparation() const {
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

	const AudioLoader *audioLoader() const {
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
	const TestParameters &testParameters() const {
		return testParameters_;
	}

	void prepareNewTest(TestParameters p) override {
		testParameters_ = std::move(p);
	}

	void playNextTrial(StimulusPlayer *p) override {
		player_ = p;
		playNextTrialCalled_ = true;
	}
	
	bool playNextTrialCalled() const {
		return playNextTrialCalled_;
	}

	void setNextStimulus(std::string s) {
		nextStimulus_ = std::move(s);
	}

	std::string nextStimulus() override {
		return nextStimulus_;
	}

	const StimulusPlayer *player() const {
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
#include <gtest/gtest.h>

class RefactoredModelTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters newTest{};
	RefactoredModel::TrialParameters trial{};
	PrescriptionReaderStub prescriptionReader{};
	BrirReaderStub brirReader{};
	SpeechPerceptionTestStub perceptionTest{};
	HearingAidFactoryStub hearingAidFactory{};
	FirFilterFactoryStub firFilterFactory{};
	std::shared_ptr<AudioFrameReaderStub> audioFrameReader = std::make_shared<AudioFrameReaderStub>();
	AudioFrameReaderStubFactory audioFrameReaderFactory{audioFrameReader};
	AudioPlayerStub player{};
	AudioLoaderStub loader{};
	RefactoredModel model{ 
		&perceptionTest,
		&prescriptionReader, 
		&brirReader, 
		&hearingAidFactory, 
		&firFilterFactory,
		&audioFrameReaderFactory,
		&player,
		&loader
	};

	void prepareNewTest() {
		model.prepareNewTest(newTest);
	}

	void playTrial() {
		model.playTrial(trial);
	}
};

TEST_F(RefactoredModelTests, constructorAssignsAudioLoaderToPlayer) {
	EXPECT_EQ(&loader, player.audioLoader());
}

TEST_F(RefactoredModelTests, prepareNewTestReadsPrescriptionsWhenUsingHearingAidSimulation) {
	newTest.usingHearingAidSimulation = true;
	newTest.leftDslPrescriptionFilePath = "a";
	newTest.rightDslPrescriptionFilePath = "b";
	prepareNewTest();
	EXPECT_TRUE(prescriptionReader.filePaths().contains("a"));
	EXPECT_TRUE(prescriptionReader.filePaths().contains("b"));
}

TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation) {
	newTest.usingHearingAidSimulation = false;
	newTest.leftDslPrescriptionFilePath = "a";
	newTest.rightDslPrescriptionFilePath = "b";
	prepareNewTest();
	EXPECT_TRUE(prescriptionReader.filePaths().empty());
}

TEST_F(RefactoredModelTests, prepareNewTestReadsBrirWhenUsingSpatialization) {
	newTest.usingSpatialization = true;
	newTest.brirFilePath = "a";
	prepareNewTest();
	assertEqual("a", brirReader.filePath());
}

TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
	newTest.usingSpatialization = false;
	newTest.brirFilePath = "a";
	prepareNewTest();
	EXPECT_TRUE(brirReader.filePath().empty());
}

TEST_F(RefactoredModelTests, prepareNewTestPassesParametersToSpeechPerceptionTest) {
	newTest.audioDirectory = "a";
	newTest.testFilePath = "b";
	prepareNewTest();
	assertEqual("a", perceptionTest.testParameters().audioDirectory);
	assertEqual("b", perceptionTest.testParameters().testFilePath);
}

TEST_F(RefactoredModelTests, playTrialPassesStimulusPlayerToSpeechPerceptionTest) {
	playTrial();
	EXPECT_EQ(&player, perceptionTest.player());
}

TEST_F(RefactoredModelTests, playTrialPassesAudioFilePathToFactory) {
	perceptionTest.setNextStimulus("a");
	playTrial();
	assertEqual("a", audioFrameReaderFactory.filePath());
}

TEST_F(RefactoredModelTests, playTrialPassesAudioReaderToAudioLoader) {
	playTrial();
	EXPECT_EQ(audioFrameReader, loader.audioFrameReader());
}

TEST_F(RefactoredModelTests, playTrialPassesParametersToPlayer) {
	newTest.usingHearingAidSimulation = true;
	newTest.chunkSize = 3;
	prepareNewTest();
	audioFrameReader->setChannels(1);
	audioFrameReader->setSampleRate(2);
	trial.audioDevice = "a";
	playTrial();
	EXPECT_EQ(1, player.preparation().channels);
	EXPECT_EQ(2, player.preparation().sampleRate);
	EXPECT_EQ(3, player.preparation().framesPerBuffer);
	assertEqual("a", player.preparation().audioDevice);
}

TEST_F(RefactoredModelTests, playTrialResetsReaderAfterComputingRms) {
	playTrial();
	EXPECT_TRUE(audioFrameReader->readingLog().endsWith("reset "));
}

TEST_F(RefactoredModelTests, DISABLED_playTrialComputesCalibrationScalars) {
	//FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
	//fakeReader.setChannels(2);
	//setInMemoryReader(fakeReader);
	//preparation.level_dB_Spl = 7;
	//processorFactory.setFullScale_dB_Spl(8);
	playTrial();
//		auto desiredRms = std::pow(10.0, (7 - 8) / 20.0);
	/*assertEqual(
		{
			desiredRms / rms<float>({ 1, 3, 5 }),
			desiredRms / rms<float>({ 2, 4, 6 })
		},
		processorFactory.parameters().channelScalars,
		1e-6
	);*/
}

TEST_F(RefactoredModelTests, playTrialPassesCompressionParametersToHearingAidFactory) {
	PrescriptionReader::Dsl leftPrescription;
	leftPrescription.compressionRatios = { 1 };
	leftPrescription.crossFrequenciesHz = { 2 };
	leftPrescription.kneepointGains_dB = { 3 };
	leftPrescription.kneepoints_dBSpl = { 4 };
	leftPrescription.broadbandOutputLimitingThresholds_dBSpl = { 5 };
	leftPrescription.channels = 6;
	prescriptionReader.addPrescription("leftFilePath", leftPrescription);
	PrescriptionReader::Dsl rightPrescription;
	rightPrescription.compressionRatios = { 1, 1 };
	rightPrescription.crossFrequenciesHz = { 2, 2 };
	rightPrescription.kneepointGains_dB = { 3, 3 };
	rightPrescription.kneepoints_dBSpl = { 4, 4 };
	rightPrescription.broadbandOutputLimitingThresholds_dBSpl = { 5, 5 };
	rightPrescription.channels = 12;
	prescriptionReader.addPrescription("rightFilePath", rightPrescription);
	audioFrameReader->setSampleRate(7);
	newTest.usingHearingAidSimulation = true;
	newTest.attack_ms = 8;
	newTest.release_ms = 9;
	newTest.chunkSize = 10;
	newTest.windowSize = 11;
	newTest.leftDslPrescriptionFilePath = "leftFilePath";
	newTest.rightDslPrescriptionFilePath = "rightFilePath";
	prepareNewTest();
	playTrial();
	auto left = hearingAidFactory.parameters().at(0);
	assertEqual({ 1 }, left.compressionRatios);
	assertEqual({ 2 }, left.crossFrequenciesHz);
	assertEqual({ 3 }, left.kneepointGains_dB);
	assertEqual({ 4 }, left.kneepoints_dBSpl);
	assertEqual({ 5 }, left.broadbandOutputLimitingThresholds_dBSpl);
	EXPECT_EQ(6, left.channels);
	EXPECT_EQ(7, left.sampleRate);
	EXPECT_EQ(8, left.attack_ms);
	EXPECT_EQ(9, left.release_ms);
	EXPECT_EQ(10, left.chunkSize);
	EXPECT_EQ(11, left.windowSize);
	auto right = hearingAidFactory.parameters().at(1);
	assertEqual({ 1, 1 }, right.compressionRatios);
	assertEqual({ 2, 2 }, right.crossFrequenciesHz);
	assertEqual({ 3, 3 }, right.kneepointGains_dB);
	assertEqual({ 4, 4 }, right.kneepoints_dBSpl);
	assertEqual({ 5, 5 }, right.broadbandOutputLimitingThresholds_dBSpl);
	EXPECT_EQ(12, right.channels);
	EXPECT_EQ(7, right.sampleRate);
	EXPECT_EQ(8, right.attack_ms);
	EXPECT_EQ(9, right.release_ms);
	EXPECT_EQ(10, right.chunkSize);
	EXPECT_EQ(11, right.windowSize);
}

TEST_F(RefactoredModelTests, playTrialPassesBrirToFirFilterFactory) {
	BrirReader::BinauralRoomImpulseResponse brir;
	brir.left = { 1, 2 };
	brir.right = { 3, 4 };
	brirReader.setBrir(brir);
	newTest.usingSpatialization = true;
	prepareNewTest();
	playTrial();
	EXPECT_TRUE(firFilterFactory.coefficients().contains({ 1, 2 }));
	EXPECT_TRUE(firFilterFactory.coefficients().contains({ 3, 4 }));
}

TEST_F(RefactoredModelTests, audioDeviceDescriptionsReturnsDescriptionsFromPlayer) {
	player.setAudioDeviceDescriptions({ "a", "b", "c" });
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
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
			prescriptionReader, 
			brirReader, 
			hearingAidFactory, 
			firFilterFactory,
			audioReaderFactory,
			player,
			loader
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