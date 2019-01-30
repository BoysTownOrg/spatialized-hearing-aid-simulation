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
	AudioFrameReaderFactory *readerFactory;
	AudioStimulusPlayer *player;
	AudioLoader *loader;
public:
	RefactoredModel(
		SpeechPerceptionTest *test,
		PrescriptionReader *prescriptionReader,
		BrirReader *brirReader,
		FilterbankCompressorFactory *compressorFactory,
		AudioFrameReaderFactory *readerFactory,
		AudioStimulusPlayer *player,
		AudioLoader *loader
	) :
		prescriptionReader{ prescriptionReader },
		brirReader{ brirReader },
		test{ test },
		compressorFactory{ compressorFactory },
		readerFactory{ readerFactory },
		player{ player },
		loader{ loader }
	{
		player->setAudioLoader(loader);
	}

	void prepareNewTest(TestParameters p) override {
		prepareNewTest_(p);
		if (p.usingSpatialization)
			readBrir(p);
		if (p.usingHearingAidSimulation)
			readPrescriptions(p);
		testParameters = p;
	}

	void playTrial(TrialParameters p) override {
		auto reader = readerFactory->make(test->nextStimulus());
		loader->setReader(reader);
		prepareAudioPlayer(*reader);
		test->playNextTrial(player);
		makeCompressor(
			prescriptionReader->read(testParameters.leftDslPrescriptionFilePath), 
			reader->sampleRate()
		);
		makeCompressor(
			prescriptionReader->read(testParameters.rightDslPrescriptionFilePath), 
			reader->sampleRate()
		);
		reader->reset();
	}

	bool testComplete() override {
		return false;
	}

	void playCalibration(CalibrationParameters) override {
		try {
			player->prepareToPlay({});
		}
		catch (const IAudioPlayer::PreparationFailure &e) {
			throw CalibrationFailure{ e.what() };
		}
	}

	void stopCalibration() override {
	}

	std::vector<std::string> audioDeviceDescriptions() override { 
		return player->audioDeviceDescriptions(); 
	}

private:
	void readPrescriptions(Model::TestParameters p) {
		readPrescription(p.leftDslPrescriptionFilePath);
		readPrescription(p.rightDslPrescriptionFilePath);
	}

	PrescriptionReader::Dsl readPrescription(std::string filePath) {
		try {
			return prescriptionReader->read(filePath);
		}
		catch (const PrescriptionReader::ReadFailure &) {
			throw TestInitializationFailure{ "Unable to read '" + filePath + "'." };
		}
	}

	BrirReader::BinauralRoomImpulseResponse readBrir(Model::TestParameters p) {
		try {
			return brirReader->read(p.brirFilePath);
		}
		catch (const BrirReader::ReadFailure &) {
			throw TestInitializationFailure{ "Unable to read '" + p.brirFilePath + "'." };
		}
	}

	std::shared_ptr<FilterbankCompressor> makeCompressor(PrescriptionReader::Dsl dsl, int sampleRate) {
		FilterbankCompressor::Parameters compression;
		compression.attack_ms = testParameters.attack_ms;
		compression.release_ms = testParameters.release_ms;
		compression.chunkSize = testParameters.chunkSize;
		compression.windowSize = testParameters.windowSize;

		compression.sampleRate = sampleRate;
		compression.compressionRatios = dsl.compressionRatios;
		compression.crossFrequenciesHz = dsl.crossFrequenciesHz;
		compression.kneepointGains_dB = dsl.kneepointGains_dB;
		compression.kneepoints_dBSpl = dsl.kneepoints_dBSpl;
		compression.broadbandOutputLimitingThresholds_dBSpl = dsl.broadbandOutputLimitingThresholds_dBSpl;
		compression.channels = dsl.channels;
		return compressorFactory->make(compression);
	}

	void prepareAudioPlayer(AudioFrameReader &reader) {
		IAudioPlayer::Preparation playing{};
		playing.channels = reader.channels();
		playing.framesPerBuffer = testParameters.chunkSize;
		playing.sampleRate = reader.sampleRate();
		try {
			player->prepareToPlay(playing);
		}
		catch (const IAudioPlayer::PreparationFailure &e) {
			throw TrialFailure{ e.what() };
		}
	}

	void prepareNewTest_(Model::TestParameters p) {
		SpeechPerceptionTest::TestParameters adapted;
		adapted.audioDirectory = p.audioDirectory;
		adapted.testFilePath = p.testFilePath;
		try {
			test->prepareNewTest(adapted);
		}
		catch (const SpeechPerceptionTest::TestInitializationFailure &e) {
			throw TestInitializationFailure{ e.what() };
		}
	}
};

#include "ArgumentCollection.h"
#include <map>

class PrescriptionReaderStub : public PrescriptionReader {
	ArgumentCollection<std::string> filePaths_{};
	std::map<std::string, Dsl> prescriptions_{};
public:
	void addPrescription(std::string filePath, Dsl dsl) {
		prescriptions_[filePath] = dsl;
	}

	Dsl read(std::string filePath) override {
		filePaths_.push_back(filePath);
		return prescriptions_[filePath];
	}

	ArgumentCollection<std::string> filePaths() const {
		return filePaths_;
	}
};

class FailingPrescriptionReader : public PrescriptionReader {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	Dsl read(std::string) override {
		throw ReadFailure{ errorMessage };
	}
};

class BrirReaderStub : public BrirReader {
	std::string filePath_{};
public:
	BinauralRoomImpulseResponse read(std::string filePath) override {
		filePath_ = std::move(filePath);
		return {};
	}

	std::string filePath() const {
		return filePath_;
	}
};

class FailingBrirReader : public BrirReader {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	BinauralRoomImpulseResponse read(std::string) override {
		throw ReadFailure{ errorMessage };
	}
};

class AudioPlayerStub : public AudioStimulusPlayer {
	std::vector<std::string> audioDeviceDescriptions_{};
	Preparation preparation_{};
	AudioLoader *audioLoader_{};
public:
	void prepareToPlay(Preparation p) override
	{
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
#include <gtest/gtest.h>

class RefactoredModelTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	RefactoredModel::TrialParameters trial{};
	PrescriptionReaderStub prescriptionReader{};
	BrirReaderStub brirReader{};
	SpeechPerceptionTestStub test{};
	FilterbankCompressorSpyFactory compressorFactory{};
	std::shared_ptr<AudioFrameReaderStub> audioFrameReader = std::make_shared<AudioFrameReaderStub>();
	AudioFrameReaderStubFactory audioFrameReaderFactory{audioFrameReader};
	AudioPlayerStub player{};
	AudioLoaderStub loader{};
	RefactoredModel model{ 
		&test,
		&prescriptionReader, 
		&brirReader, 
		&compressorFactory, 
		&audioFrameReaderFactory,
		&player,
		&loader
	};

	void prepareNewTest() {
		model.prepareNewTest(testing);
	}

	void playTrial() {
		model.playTrial(trial);
	}
};

TEST_F(RefactoredModelTests, constructorAssignsAudioLoaderToPlayer) {
	EXPECT_EQ(&loader, player.audioLoader());
}

TEST_F(RefactoredModelTests, prepareNewTestReadsPrescriptionsWhenUsingHearingAidSimulation) {
	testing.usingHearingAidSimulation = true;
	testing.leftDslPrescriptionFilePath = "a";
	testing.rightDslPrescriptionFilePath = "b";
	prepareNewTest();
	EXPECT_TRUE(prescriptionReader.filePaths().contains("a"));
	EXPECT_TRUE(prescriptionReader.filePaths().contains("b"));
}

TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation) {
	testing.usingHearingAidSimulation = false;
	testing.leftDslPrescriptionFilePath = "a";
	testing.rightDslPrescriptionFilePath = "b";
	prepareNewTest();
	EXPECT_TRUE(prescriptionReader.filePaths().empty());
}

TEST_F(RefactoredModelTests, prepareNewTestReadsBrirWhenUsingSpatialization) {
	testing.usingSpatialization = true;
	testing.brirFilePath = "a";
	prepareNewTest();
	assertEqual("a", brirReader.filePath());
}

TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
	testing.usingSpatialization = false;
	testing.brirFilePath = "a";
	prepareNewTest();
	EXPECT_TRUE(brirReader.filePath().empty());
}

TEST_F(RefactoredModelTests, prepareNewTestPassesParametersToSpeechPerceptionTest) {
	testing.audioDirectory = "a";
	testing.testFilePath = "b";
	prepareNewTest();
	assertEqual("a", test.testParameters().audioDirectory);
	assertEqual("b", test.testParameters().testFilePath);
}

TEST_F(RefactoredModelTests, playTrialPassesStimulusPlayerToSpeechPerceptionTest) {
	playTrial();
	EXPECT_EQ(&player, test.player());
}

TEST_F(RefactoredModelTests, playTrialPassesAudioFilePathToFactory) {
	test.setNextStimulus("a");
	playTrial();
	assertEqual("a", audioFrameReaderFactory.filePath());
}

TEST_F(RefactoredModelTests, playTrialPassesAudioReaderToAudioLoader) {
	playTrial();
	EXPECT_EQ(audioFrameReader, loader.audioFrameReader());
}

TEST_F(RefactoredModelTests, playTrialPassesParametersToPlayer) {
	testing.usingHearingAidSimulation = true;
	testing.chunkSize = 3;
	prepareNewTest();
	audioFrameReader->setChannels(1);
	audioFrameReader->setSampleRate(2);
	playTrial();
	EXPECT_EQ(1, player.preparation().channels);
	EXPECT_EQ(2, player.preparation().sampleRate);
	EXPECT_EQ(3, player.preparation().framesPerBuffer);
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

TEST_F(RefactoredModelTests, playTrialPassesCompressionParametersToFactory) {
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
	testing.usingHearingAidSimulation = true;
	testing.attack_ms = 8;
	testing.release_ms = 9;
	testing.chunkSize = 10;
	testing.windowSize = 11;
	testing.leftDslPrescriptionFilePath = "leftFilePath";
	testing.rightDslPrescriptionFilePath = "rightFilePath";
	prepareNewTest();
	playTrial();
	auto left = compressorFactory.parameters().at(0);
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
	auto right = compressorFactory.parameters().at(1);
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

TEST_F(RefactoredModelTests, audioDeviceDescriptionsReturnsDescriptionsFromPlayer) {
	player.setAudioDeviceDescriptions({ "a", "b", "c" });
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
}

class RefactoredModelFailureTests : public ::testing::Test {
protected:
	RefactoredModel::TestParameters testing{};
	PrescriptionReaderStub defaultPrescriptionReader{};
	PrescriptionReader *prescriptionReader{&defaultPrescriptionReader};
	BrirReaderStub defaultBrirReader{};
	BrirReader *brirReader{&defaultBrirReader};
	SpeechPerceptionTestStub defaultTest{};
	SpeechPerceptionTest *test{&defaultTest};
	FilterbankCompressorSpyFactory defaultCompressorFactory{};
	FilterbankCompressorFactory *compressorFactory{&defaultCompressorFactory};
	AudioFrameReaderStubFactory defaultAudioReaderFactory{};
	AudioFrameReaderFactory *audioReaderFactory{&defaultAudioReaderFactory};
	AudioPlayerStub defaultPlayer{};
	AudioStimulusPlayer *player{ &defaultPlayer };
	AudioLoaderStub defaultLoader{};
	AudioLoader *loader{ &defaultLoader };

	void assertPreparingNewTestThrowsTestInitializationFailure(std::string what) {
		auto model = makeModel();
		try {
			model.prepareNewTest(testing);
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
			compressorFactory, 
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
	testing.usingHearingAidSimulation = true;
	testing.leftDslPrescriptionFilePath = "a";
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
	testing.usingSpatialization = true;
	testing.brirFilePath = "a";
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