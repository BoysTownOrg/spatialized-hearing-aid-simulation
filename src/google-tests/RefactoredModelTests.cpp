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
	template<typename T>
	class PoppableVector {
		std::vector<T> elements;
	public:
		PoppableVector(typename std::vector<T>::size_type count) : elements(count) {}

		void set(std::vector<T> v) noexcept {
			elements = std::move(v);
		}

		T pop_front() {
			auto item = elements.front();
			elements.erase(elements.begin());
			return item;
		}
	};
	class SpatializedHearingAidSimulationFactoryStub : 
		public ISpatializedHearingAidSimulationFactory 
	{
		ArgumentCollection<HearingAidSimulation> fullSimulationHearingAid_{};
		ArgumentCollection<HearingAidSimulation> hearingAidSimulation_{};
		ArgumentCollection<Spatialization> fullSimulationSpatialization_{};
		ArgumentCollection<Spatialization> spatialization_{};
		ArgumentCollection<float> fullSimulationScale_{};
		ArgumentCollection<float> hearingAidSimulationScale_{};
		ArgumentCollection<float> spatializationScale_{};
		ArgumentCollection<float> withoutSimulationScale_{};
		PoppableVector<std::shared_ptr<SignalProcessor>> fullSimulationProcessors;
		PoppableVector<std::shared_ptr<SignalProcessor>> hearingAidSimulationProcessors;
		PoppableVector<std::shared_ptr<SignalProcessor>> spatializationProcessors;
		PoppableVector<std::shared_ptr<SignalProcessor>> withoutSimulationProcessors;
	public:
		SpatializedHearingAidSimulationFactoryStub() :
			fullSimulationProcessors(2),
			hearingAidSimulationProcessors(2),
			spatializationProcessors(2),
			withoutSimulationProcessors(2) {}

		void setFullSimulationProcessors(std::vector<std::shared_ptr<SignalProcessor>> p) noexcept {
			fullSimulationProcessors.set(std::move(p));
		}

		void setHearingAidSimulationProcessors(std::vector<std::shared_ptr<SignalProcessor>> p) noexcept {
			hearingAidSimulationProcessors.set(std::move(p));
		}

		void setSpatializationProcessors(std::vector<std::shared_ptr<SignalProcessor>> p) noexcept {
			spatializationProcessors.set(std::move(p));
		}

		void setWithoutSimulationProcessors(std::vector<std::shared_ptr<SignalProcessor>> p) noexcept {
			withoutSimulationProcessors.set(std::move(p));
		}

		std::shared_ptr<SignalProcessor> makeFullSimulation(
			FullSimulation s, float x
		) override {
			fullSimulationHearingAid_.push_back(std::move(s.hearingAid));
			fullSimulationSpatialization_.push_back(std::move(s.spatialization));
			fullSimulationScale_.push_back(x);
			return fullSimulationProcessors.pop_front();
		}

		std::shared_ptr<SignalProcessor> makeHearingAidSimulation(
			HearingAidSimulation s, float x
		) override {
			hearingAidSimulation_.push_back(std::move(s));
			hearingAidSimulationScale_.push_back(x);
			return hearingAidSimulationProcessors.pop_front();
		}

		std::shared_ptr<SignalProcessor> makeSpatialization(
			Spatialization s, float x
		) override {
			spatialization_.push_back(std::move(s));
			spatializationScale_.push_back(x);
			return spatializationProcessors.pop_front();
		}

		std::shared_ptr<SignalProcessor> makeWithoutSimulation(
			float x
		) override {
			withoutSimulationScale_.push_back(x);
			return withoutSimulationProcessors.pop_front();
		}

		auto fullSimulationSpatialization() const {
			return fullSimulationSpatialization_;
		}
		
		auto &fullSimulationHearingAid() const {
			return fullSimulationHearingAid_;
		}

		auto &hearingAidSimulation() const {
			return hearingAidSimulation_;
		}
		auto spatialization() const {
			return spatialization_;
		}

		auto &fullSimulationScale() const {
			return fullSimulationScale_;
		}

		auto &hearingAidSimulationScale() const {
			return hearingAidSimulationScale_;
		}

		auto &spatializationScale() const {
			return spatializationScale_;
		}

		auto &withoutSimulationScale() const {
			return withoutSimulationScale_;
		}
	};

	class CalibrationComputerStub : public ICalibrationComputer {
		ArgumentCollection<double> levels_{};
		std::map<int, double> signalScales;
	public:
		double signalScale(int channel, double level) override
		{
			levels_.push_back(level);
			return signalScales[channel];
		}

		void addSignalScale(int channel, double scale) {
			signalScales[channel] = scale;
		}

		auto levels() const {
			return levels_;
		}
	};

	class CalibrationComputerStubFactory : public ICalibrationComputerFactory {
		std::shared_ptr<ICalibrationComputer> computer;
		AudioFrameReader *reader_;
	public:
		explicit CalibrationComputerStubFactory(
			std::shared_ptr<ICalibrationComputer> computer =
				std::make_shared<CalibrationComputerStub>()
		) :
			computer{ computer } {}

		std::shared_ptr<ICalibrationComputer> make(AudioFrameReader *r) override
		{
			reader_ = r;
			return computer;
		}

		auto reader() const {
			return reader_;
		}
	};

	class RefactoredModelTests : public ::testing::Test {
	protected:
		using channel_type = AudioFrameProcessor::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;

		RefactoredModel::TestParameters testParameters{};
		RefactoredModel::TrialParameters trialParameters{};
		RefactoredModel::CalibrationParameters calibrationParameters{};
		PrescriptionReaderStub prescriptionReader{};
		BrirReaderStub brirReader{};
		SpeechPerceptionTestStub perceptionTest{};
		std::shared_ptr<AudioFrameReaderStub> audioFrameReader
			= std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory audioFrameReaderFactory{ audioFrameReader };
		AudioPlayerStub audioPlayer{};
		AudioLoaderStub audioLoader{};
		SpatializedHearingAidSimulationFactoryStub simulationFactory{};
		std::shared_ptr<CalibrationComputerStub> calibrationComputer =
			std::make_shared<CalibrationComputerStub>();
		CalibrationComputerStubFactory calibrationFactory{ calibrationComputer };
		RefactoredModel model{
			&perceptionTest,
			&audioPlayer,
			&audioLoader,
			&audioFrameReaderFactory,
			&prescriptionReader,
			&brirReader,
			&simulationFactory,
			&calibrationFactory
		};

		RefactoredModelTests() {
			setValidDefaults();
		}

		void setValidDefaults() {
			BrirReader::BinauralRoomImpulseResponse brir;
			brir.left = { 0 };
			brir.right = { 0 };
			brirReader.setBrir(brir);
			testParameters.processing.chunkSize = 1;
			testParameters.processing.windowSize = 1;
		}

		void prepareNewTest() {
			model.prepareNewTest(testParameters);
		}

		void playTrial() {
			model.playTrial(trialParameters);
		}

		void playCalibration() {
			model.playCalibration(calibrationParameters);
		}

		void setInMemoryReader(AudioFileReader &reader_) {
			audioFrameReaderFactory.setReader(std::make_shared<AudioFileInMemory>(reader_));
		}

		void playFirstTrialOfNewTest() {
			prepareNewTest();
			playTrial();
		}

		void setFullSimulationForTest() noexcept {
			testParameters.processing.usingHearingAidSimulation = true;
			testParameters.processing.usingSpatialization = true;
		}

		void setHearingAidSimulationOnlyForTest() noexcept {
			testParameters.processing.usingHearingAidSimulation = true;
			testParameters.processing.usingSpatialization = false;
		}

		void setSpatializationOnlyForTest() noexcept {
			testParameters.processing.usingSpatialization = true;
			testParameters.processing.usingHearingAidSimulation = false;
		}

		void setNoSimulationForTest() noexcept {
			testParameters.processing.usingSpatialization = false;
			testParameters.processing.usingHearingAidSimulation = false;
		}

		void setFullSimulationForCalibration() noexcept {
			calibrationParameters.processing.usingHearingAidSimulation = true;
			calibrationParameters.processing.usingSpatialization = true;
		}

		void setHearingAidSimulationOnlyForCalibration() noexcept {
			calibrationParameters.processing.usingHearingAidSimulation = true;
			calibrationParameters.processing.usingSpatialization = false;
		}

		void setSpatializationOnlyForCalibration() noexcept {
			calibrationParameters.processing.usingSpatialization = true;
			calibrationParameters.processing.usingHearingAidSimulation = false;
		}

		void setNoSimulationForCalibration() noexcept {
			calibrationParameters.processing.usingSpatialization = false;
			calibrationParameters.processing.usingHearingAidSimulation = false;
		}

		void processWhenPlayerPlays(gsl::span<channel_type> channels) {
			callWhenPlayerPlays([=]() { audioLoader.audioFrameProcessor()->process(channels); });
		}

		void assertAudioLoaderHasNotBeenModified() {
			assertTrue(audioLoader.log().isEmpty());
		}

		void callWhenPlayerPlays(std::function<void(void)> f) {
			audioPlayer.callOnPlay([=]() { f(); });
		}

		void assertSimulationFactoryHasNotMadeFullSimulation() {
			assertTrue(simulationFactory.fullSimulationHearingAid().empty());
		}

		void assertSimulationFactoryHasNotMadeHearingAidSimulation() {
			assertTrue(simulationFactory.hearingAidSimulation().empty());
		}

		void assertSimulationFactoryHasNotMadeSpatialization() {
			assertTrue(simulationFactory.spatialization().empty());
		}

		void assertAudioPlayerHasBeenPlayed() {
			assertTrue(audioPlayer.played());
		}

		void assertAudioPlayerParametersMatchAudioFrameReaderAfterCall(std::function<void(void)> f) {
			audioFrameReader->setChannels(1);
			audioFrameReader->setSampleRate(2);
			f();
			assertEqual(1, audioPlayer.preparation().channels);
			assertEqual(2, audioPlayer.preparation().sampleRate);
		}

		void assertAudioPlayerFramesPerBufferMatchesProcessingChunkSizeAfterCall(
			RefactoredModel::ProcessingParameters &processing, 
			std::function<void(void)> f
		) {
			processing.chunkSize = 1;
			f();
			assertEqual(1, audioPlayer.preparation().framesPerBuffer);
		}

		void assertAudioPlayerFramesPerBufferMatchesDefault() {
			assertEqual(
				RefactoredModel::defaultFramesPerBuffer, 
				audioPlayer.preparation().framesPerBuffer
			);
		}

		void assertCalibrationFactoryReceivesAudioFrameReader() {
			EXPECT_EQ(audioFrameReader.get(), calibrationFactory.reader());
		}

		void assertCalibrationDigitalLevelsAfterCall(double &level, std::function<void(void)> f) {
			audioFrameReader->setChannels(2);
			level = 65;
			f();
			assertEqual(65 - RefactoredModel::fullScaleLevel_dB_Spl, calibrationComputer->levels().at(0));
			assertEqual(65 - RefactoredModel::fullScaleLevel_dB_Spl, calibrationComputer->levels().at(1));
		}

		void assertScalarsMatchCalibrationAfterCall(
			const ArgumentCollection<float> &scalars, 
			std::function<void(void)> f
		) {
			audioFrameReader->setChannels(2);
			calibrationComputer->addSignalScale(0, 3.3);
			calibrationComputer->addSignalScale(1, 4.4);
			f();
			assertEqual(3.3f, scalars.at(0));
			assertEqual(4.4f, scalars.at(1));
		}

		void assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			RefactoredModel::ProcessingParameters &processing,
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::HearingAidSimulation> &hearingAid,
			std::function<void(void)> f
		) {
			PrescriptionReader::Dsl left;
			left.compressionRatios = { 1 };
			left.crossFrequenciesHz = { 2 };
			left.kneepointGains_dB = { 3 };
			left.kneepoints_dBSpl = { 4 };
			left.broadbandOutputLimitingThresholds_dBSpl = { 5 };
			left.channels = 6;
			processing.leftDslPrescriptionFilePath = "leftFilePath";
			prescriptionReader.addPrescription("leftFilePath", left);

			PrescriptionReader::Dsl right;
			right.compressionRatios = { 7 };
			right.crossFrequenciesHz = { 8 };
			right.kneepointGains_dB = { 9 };
			right.kneepoints_dBSpl = { 10 };
			right.broadbandOutputLimitingThresholds_dBSpl = { 11 };
			right.channels = 12;
			processing.rightDslPrescriptionFilePath = "rightFilePath";
			prescriptionReader.addPrescription("rightFilePath", right);

			f();

			auto actualLeft = hearingAid.at(0).prescription;
			assertEqual({ 1 }, actualLeft.compressionRatios);
			assertEqual({ 2 }, actualLeft.crossFrequenciesHz);
			assertEqual({ 3 }, actualLeft.kneepointGains_dB);
			assertEqual({ 4 }, actualLeft.kneepoints_dBSpl);
			assertEqual({ 5 }, actualLeft.broadbandOutputLimitingThresholds_dBSpl);
			assertEqual(6, actualLeft.channels);

			auto actualRight = hearingAid.at(1).prescription;
			assertEqual({ 7 }, actualRight.compressionRatios);
			assertEqual({ 8 }, actualRight.crossFrequenciesHz);
			assertEqual({ 9 }, actualRight.kneepointGains_dB);
			assertEqual({ 10 }, actualRight.kneepoints_dBSpl);
			assertEqual({ 11 }, actualRight.broadbandOutputLimitingThresholds_dBSpl);
			assertEqual(12, actualRight.channels);
		}

		void assertHearingAidCompressionParametersMatchAfterCall(
			RefactoredModel::ProcessingParameters &processing,
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::HearingAidSimulation> &hearingAid,
			std::function<void(void)> f
		) {
			processing.attack_ms = 1;
			processing.release_ms = 2;
			processing.chunkSize = 4;
			processing.windowSize = 8;
			
			f();

			auto left = hearingAid.at(0);
			assertEqual(1.0, left.attack_ms);
			assertEqual(2.0, left.release_ms);
			assertEqual(4, left.chunkSize);
			assertEqual(8, left.windowSize);

			auto right = hearingAid.at(1);
			assertEqual(1.0, right.attack_ms);
			assertEqual(2.0, right.release_ms);
			assertEqual(4, right.chunkSize);
			assertEqual(8, right.windowSize);
		}

		void assertHearingAidSimulationSampleRateMatchesAudioReaderAfterCall(
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::HearingAidSimulation> &hearingAid,
			std::function<void(void)> f
		) {
			audioFrameReader->setSampleRate(1);
			f();
			assertEqual(1, hearingAid.at(0).sampleRate);
			assertEqual(1, hearingAid.at(1).sampleRate);
		}
	};

	TEST_F(RefactoredModelTests, constructorAssignsAudioLoaderToPlayer) {
		EXPECT_EQ(&audioLoader, audioPlayer.audioLoader());
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		testParameters.processing.usingHearingAidSimulation = true;
		testParameters.processing.leftDslPrescriptionFilePath = "a";
		testParameters.processing.rightDslPrescriptionFilePath = "b";
		prepareNewTest();
		assertTrue(prescriptionReader.filePaths().contains("a"));
		assertTrue(prescriptionReader.filePaths().contains("b"));
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		testParameters.processing.usingHearingAidSimulation = false;
		prepareNewTest();
		assertTrue(prescriptionReader.filePaths().empty());
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		testParameters.processing.usingSpatialization = true;
		testParameters.processing.brirFilePath = "a";
		prepareNewTest();
		assertEqual("a", brirReader.filePath());
	}

	TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
		testParameters.processing.usingSpatialization = false;
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
		assertAudioPlayerHasBeenPlayed();
	}

	TEST_F(RefactoredModelTests, playCalibrationPlaysPlayer) {
		playCalibration();
		assertAudioPlayerHasBeenPlayed();
	}

	TEST_F(RefactoredModelTests, playTrialPassesNextStimulusToFactory) {
		perceptionTest.setNextStimulus("a");
		playTrial();
		assertEqual("a", audioFrameReaderFactory.filePath());
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesAudioFileToFactory) {
		calibrationParameters.audioFilePath = "a";
		playCalibration();
		assertEqual("a", audioFrameReaderFactory.filePath());
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		callWhenPlayerPlays([=]() {
			EXPECT_EQ(audioFrameReader, audioLoader.audioFrameReader());
		});
		playTrial();
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		callWhenPlayerPlays([=]() {
			EXPECT_EQ(audioFrameReader, audioLoader.audioFrameReader());
		});
		playCalibration();
	}

	TEST_F(RefactoredModelTests, playTrialPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReaderAfterCall([=]() { playTrial(); });
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReaderAfterCall([=]() { playCalibration(); });
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioDeviceToPlayer) {
		trialParameters.audioDevice = "a";
		playTrial();
		assertEqual("a", audioPlayer.preparation().audioDevice);
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesAudioDeviceToPlayer) {
		calibrationParameters.audioDevice = "a";
		playCalibration();
		assertEqual("a", audioPlayer.preparation().audioDevice);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation
	) {
		testParameters.processing.usingHearingAidSimulation = true;
		assertAudioPlayerFramesPerBufferMatchesProcessingChunkSizeAfterCall(
			testParameters.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation
	) {
		calibrationParameters.processing.usingHearingAidSimulation = true;
		assertAudioPlayerFramesPerBufferMatchesProcessingChunkSizeAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests,
		playTrialUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		testParameters.processing.usingHearingAidSimulation = false;
		playFirstTrialOfNewTest();
		assertAudioPlayerFramesPerBufferMatchesDefault();
	}

	TEST_F(
		RefactoredModelTests,
		playCalibrationUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		calibrationParameters.processing.usingHearingAidSimulation = false;
		playCalibration();
		assertAudioPlayerFramesPerBufferMatchesDefault();
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioFrameReaderToCalibrationFactory) {
		playTrial();
		assertCalibrationFactoryReceivesAudioFrameReader();
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesAudioFrameReaderToCalibrationFactory) {
		playCalibration();
		assertCalibrationFactoryReceivesAudioFrameReader();
	}

	TEST_F(RefactoredModelTests, playTrialPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevelsAfterCall(
			trialParameters.level_dB_Spl,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevelsAfterCall(
			calibrationParameters.level_dB_Spl,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForTest();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.fullSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForCalibration();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.fullSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForTest();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForCalibration();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForTest();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.spatializationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForCalibration();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.spatializationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForTest();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.withoutSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForCalibration();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.withoutSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForTest();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			testParameters.processing, 
			simulationFactory.hearingAidSimulation(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForCalibration();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			calibrationParameters.processing, 
			simulationFactory.hearingAidSimulation(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			testParameters.processing, 
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			calibrationParameters.processing, 
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		testParameters.processing.usingHearingAidSimulation = false;
		playFirstTrialOfNewTest();
		assertSimulationFactoryHasNotMadeFullSimulation();
		assertSimulationFactoryHasNotMadeHearingAidSimulation();
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		calibrationParameters.processing.usingHearingAidSimulation = false;
		playCalibration();
		assertSimulationFactoryHasNotMadeFullSimulation();
		assertSimulationFactoryHasNotMadeHearingAidSimulation();
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		testParameters.processing.usingSpatialization = false;
		playFirstTrialOfNewTest();
		assertSimulationFactoryHasNotMadeFullSimulation();
		assertSimulationFactoryHasNotMadeSpatialization();
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		calibrationParameters.processing.usingSpatialization = false;
		playCalibration();
		assertSimulationFactoryHasNotMadeFullSimulation();
		assertSimulationFactoryHasNotMadeSpatialization();
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForTest();
		assertHearingAidCompressionParametersMatchAfterCall(
			testParameters.processing,
			simulationFactory.hearingAidSimulation(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForCalibration();
		assertHearingAidCompressionParametersMatchAfterCall(
			calibrationParameters.processing,
			simulationFactory.hearingAidSimulation(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesCompressionParametersToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertHearingAidCompressionParametersMatchAfterCall(
			testParameters.processing,
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertHearingAidCompressionParametersMatchAfterCall(
			calibrationParameters.processing,
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForTest();
		assertHearingAidSimulationSampleRateMatchesAudioReaderAfterCall(
			simulationFactory.hearingAidSimulation(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForCalibration();
		assertHearingAidSimulationSampleRateMatchesAudioReaderAfterCall(
			simulationFactory.hearingAidSimulation(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertHearingAidSimulationSampleRateMatchesAudioReaderAfterCall(
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertHearingAidSimulationSampleRateMatchesAudioReaderAfterCall(
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialAssignsFullSimulationProcessorsToAudioLoader) {
		std::vector<std::shared_ptr<SignalProcessor>> fullSimulation = {
			std::make_shared<MultipliesSamplesBy>(2.0f),
			std::make_shared<MultipliesSamplesBy>(3.0f)
		};
		simulationFactory.setFullSimulationProcessors(fullSimulation);
		buffer_type left = { 5 };
		buffer_type right = { 7 };
		std::vector<channel_type> channels = { left, right };
		processWhenPlayerPlays(channels);
		setFullSimulationForTest();
		playFirstTrialOfNewTest();
		assertEqual({ 5 * 2 }, left);
		assertEqual({ 7 * 3 }, right);
	}

	TEST_F(RefactoredModelTests, playTrialAssignsHearingAidSimulationProcessorsToAudioLoader) {
		std::vector<std::shared_ptr<SignalProcessor>> hearingAidSimulation = {
			std::make_shared<MultipliesSamplesBy>(2.0f),
			std::make_shared<MultipliesSamplesBy>(3.0f)
		};
		simulationFactory.setHearingAidSimulationProcessors(hearingAidSimulation);
		buffer_type left = { 5 };
		buffer_type right = { 7 };
		std::vector<channel_type> channels = { left, right };
		processWhenPlayerPlays(channels);
		setHearingAidSimulationOnlyForTest();
		playFirstTrialOfNewTest();
		assertEqual({ 5 * 2 }, left);
		assertEqual({ 7 * 3 }, right);
	}

	TEST_F(RefactoredModelTests, playTrialAssignsSpatializationProcessorsToAudioLoader) {
		std::vector<std::shared_ptr<SignalProcessor>> spatialization = {
			std::make_shared<MultipliesSamplesBy>(2.0f),
			std::make_shared<MultipliesSamplesBy>(3.0f)
		};
		simulationFactory.setSpatializationProcessors(spatialization);
		buffer_type left = { 5 };
		buffer_type right = { 7 };
		std::vector<channel_type> channels = { left, right };
		processWhenPlayerPlays(channels);
		setSpatializationOnlyForTest();
		playFirstTrialOfNewTest();
		assertEqual({ 5 * 2 }, left);
		assertEqual({ 7 * 3 }, right);
	}

	TEST_F(RefactoredModelTests, playTrialAssignsWithoutSimulationProcessorsToAudioLoader) {
		std::vector<std::shared_ptr<SignalProcessor>> withoutSimulation = {
			std::make_shared<MultipliesSamplesBy>(2.0f),
			std::make_shared<MultipliesSamplesBy>(3.0f)
		};
		simulationFactory.setWithoutSimulationProcessors(withoutSimulation);
		buffer_type left = { 5 };
		buffer_type right = { 7 };
		std::vector<channel_type> channels = { left, right };
		processWhenPlayerPlays(channels);
		setNoSimulationForTest();
		playFirstTrialOfNewTest();
		assertEqual({ 5 * 2 }, left);
		assertEqual({ 7 * 3 }, right);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForTest();
		playFirstTrialOfNewTest();
		assertEqual(
			RefactoredModel::fullScaleLevel_dB_Spl, 
			simulationFactory.hearingAidSimulation().at(0).fullScaleLevel_dB_Spl
		);
		assertEqual(
			RefactoredModel::fullScaleLevel_dB_Spl, 
			simulationFactory.hearingAidSimulation().at(1).fullScaleLevel_dB_Spl
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesFullScaleLevelToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		playFirstTrialOfNewTest();
		assertEqual(
			RefactoredModel::fullScaleLevel_dB_Spl, 
			simulationFactory.fullSimulationHearingAid().at(0).fullScaleLevel_dB_Spl
		);
		assertEqual(
			RefactoredModel::fullScaleLevel_dB_Spl, 
			simulationFactory.fullSimulationHearingAid().at(1).fullScaleLevel_dB_Spl
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesBrirToFactoryForSpatialization
	) {
		BrirReader::BinauralRoomImpulseResponse brir;
		brir.left = { 1, 2 };
		brir.right = { 3, 4 };
		brirReader.setBrir(brir);
		setSpatializationOnlyForTest();
		playFirstTrialOfNewTest();
		assertEqual({ 1, 2, }, simulationFactory.spatialization().at(0).filterCoefficients);
		assertEqual({ 3, 4, }, simulationFactory.spatialization().at(1).filterCoefficients);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesBrirToFactoryForFullSimulation
	) {
		BrirReader::BinauralRoomImpulseResponse brir;
		brir.left = { 1, 2 };
		brir.right = { 3, 4 };
		brirReader.setBrir(brir);
		setFullSimulationForTest();
		playFirstTrialOfNewTest();
		assertEqual({ 1, 2, }, simulationFactory.fullSimulationSpatialization().at(0).filterCoefficients);
		assertEqual({ 3, 4, }, simulationFactory.fullSimulationSpatialization().at(1).filterCoefficients);
	}

	TEST_F(RefactoredModelTests, playTrialResetsAudioLoaderBeforePlaying) {
		callWhenPlayerPlays([=]() { assertTrue(audioLoader.log().contains("reset")); });
		playTrial();
	}

	TEST_F(RefactoredModelTests, playTrialPreparesPlayerBeforePlaying) {
		playTrial();
		assertEqual("prepareToPlay play ", audioPlayer.log());
	}

	TEST_F(RefactoredModelTests, playTrialDoesNotAlterLoaderWhenPlayerPlaying) {
		audioPlayer.setPlaying();
		playTrial();
		assertAudioLoaderHasNotBeenModified();
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
		RefactoredModel::CalibrationParameters calibrationParameters{};
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
		CalibrationComputerStubFactory defaultCalibrationFactory{};
		ICalibrationComputerFactory *calibrationFactory{ &defaultCalibrationFactory };

		void assertPreparingNewTestThrowsRequestFailure(std::string what) {
			auto model = makeModel();
			try {
				model.prepareNewTest(testParameters);
				FAIL() << "Expected RefactoredModel::RequestFailure.";
			}
			catch (const RefactoredModel::RequestFailure & e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayTrialThrowsRequestFailure(std::string what) {
			auto model = makeModel();
			try {
				model.playTrial({});
				FAIL() << "Expected RefactoredModel::RequestFailure.";
			}
			catch (const RefactoredModel::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayCalibrationThrowsRequestFailure(std::string what) {
			auto model = makeModel();
			try {
				model.playCalibration(calibrationParameters);
				FAIL() << "Expected RefactoredModel::RequestFailure.";
			}
			catch (const RefactoredModel::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void prepareNewTestIgnoringFailure() {
			auto model = makeModel();
			try {
				model.prepareNewTest(testParameters);
			}
			catch (const RefactoredModel::RequestFailure &) {
			}
		}

		void playTrialIgnoringFailure() {
			try {
				auto model = makeModel();
				model.playTrial({});
			}
			catch (const RefactoredModel::RequestFailure &) {
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
				simulationFactory,
				calibrationFactory
			};
		}
	};

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		testParameters.processing.usingHearingAidSimulation = true;
		testParameters.processing.leftDslPrescriptionFilePath = "a";
		assertPreparingNewTestThrowsRequestFailure("Unable to read 'a'.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		calibrationParameters.processing.usingHearingAidSimulation = true;
		calibrationParameters.processing.leftDslPrescriptionFilePath = "a";
		assertPlayCalibrationThrowsRequestFailure("Unable to read 'a'.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotPrepareTestWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		testParameters.processing.usingHearingAidSimulation = true;
		prepareNewTestIgnoringFailure();
		assertFalse(defaultPerceptionTest.prepareNewTestCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		testParameters.processing.usingSpatialization = true;
		testParameters.processing.brirFilePath = "a";
		assertPreparingNewTestThrowsRequestFailure("Unable to read 'a'.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		calibrationParameters.processing.usingSpatialization = true;
		calibrationParameters.processing.brirFilePath = "a";
		assertPlayCalibrationThrowsRequestFailure("Unable to read 'a'.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotPrepareTestWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		testParameters.processing.usingSpatialization = true;
		prepareNewTestIgnoringFailure();
		assertFalse(defaultPerceptionTest.prepareNewTestCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenPerceptionTestFailsToInitialize
	) {
		InitializationFailingSpeechPerceptionTest failing;
		failing.setErrorMessage("error.");
		perceptionTest = &failing;
		assertPreparingNewTestThrowsRequestFailure("error.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenCoefficientsAreEmpty
	) {
		testParameters.processing.usingSpatialization = true;
		BrirReader::BinauralRoomImpulseResponse brir;
		brir.left = {};
		brir.right = { 0 };
		defaultBrirReader.setBrir(brir);
		assertPreparingNewTestThrowsRequestFailure(
			"The left BRIR coefficients are empty, therefore a filter operation cannot be defined."
		);
		brir.left = { 0 };
		brir.right = {};
		defaultBrirReader.setBrir(brir);
		assertPreparingNewTestThrowsRequestFailure(
			"The right BRIR coefficients are empty, therefore a filter operation cannot be defined."
		);
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenWindowOrChunkSizeIsNotPowerOfTwo
	) {
		testParameters.processing.usingHearingAidSimulation = true;
		testParameters.processing.chunkSize = 0;
		testParameters.processing.windowSize = 1;
		assertPreparingNewTestThrowsRequestFailure(
			"Both the chunk size and window size must be powers of two; 0 is not a power of two."
		);
		testParameters.processing.chunkSize = 2;
		testParameters.processing.windowSize = 3;
		assertPreparingNewTestThrowsRequestFailure(
			"Both the chunk size and window size must be powers of two; 3 is not a power of two."
		);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playTrialThrowsRequestFailureWhenAudioFrameReaderCannotBeCreated
	) {
		ErrorAudioFrameReaderFactory failing{ "error." };
		audioReaderFactory = &failing;
		assertPlayTrialThrowsRequestFailure("error.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenAudioFrameReaderCannotBeCreated
	) {
		ErrorAudioFrameReaderFactory failing{ "error." };
		audioReaderFactory = &failing;
		assertPlayCalibrationThrowsRequestFailure("error.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playTrialThrowsRequestFailureWhenPlayerThrowsPreparationFailure
	) {
		PreparationFailingAudioPlayer failing;
		failing.setErrorMessage("error.");
		audioPlayer = &failing;
		assertPlayTrialThrowsRequestFailure("error.");
	}

	TEST_F(RefactoredModelFailureTests, playTrialDoesNotAdvancePerceptionTestTrialWhenPlayerFails) {
		PreparationFailingAudioPlayer failing;
		audioPlayer = &failing;
		playTrialIgnoringFailure();
		assertFalse(defaultPerceptionTest.advanceTrialCalled());
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenPlayerThrowsPreparationFailure
	) {
		PreparationFailingAudioPlayer failing;
		failing.setErrorMessage("error.");
		audioPlayer = &failing;
		assertPlayCalibrationThrowsRequestFailure("error.");
	}
}