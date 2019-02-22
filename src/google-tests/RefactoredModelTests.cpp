#include "FilterbankCompressorSpy.h"
#include "AudioFrameReaderStub.h"
#include "AudioLoaderStub.h"
#include "PrescriptionReaderStub.h"
#include "BrirReaderStub.h"
#include "FakeAudioFileReader.h"
#include "SignalProcessorStub.h"
#include "AudioPlayerStub.h"
#include "FakeStimulusList.h"
#include "DocumenterStub.h"
#include "CalibrationComputerStub.h"
#include "SpatializedHearingAidSimulationFactoryStub.h"
#include "assert-utility.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>
#include <gtest/gtest.h>

namespace {
	class RefactoredModelTests : public ::testing::Test {
	protected:
		using channel_type = AudioFrameProcessor::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;

		RefactoredModel::TestParameters testParameters{};
		RefactoredModel::TrialParameters trialParameters{};
		RefactoredModel::CalibrationParameters calibrationParameters{};
		RefactoredModel::SaveAudioParameters saveAudioParameters{};
		PrescriptionReaderStub prescriptionReader{};
		BrirReaderStub brirReader{};
		FakeStimulusList list{};
		DocumenterStub documenter{};
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
			&list,
			&documenter,
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
			setValidProcessingSizes(testParameters.processing);
			setValidProcessingSizes(calibrationParameters.processing);
			setValidProcessingSizes(saveAudioParameters.processing);
		}

		void setValidProcessingSizes(RefactoredModel::ProcessingParameters &p) noexcept {
			p.chunkSize = 1;
			p.windowSize = 1;
		}

		void playFirstTrialOfNewTest() {
			prepareNewTest();
			playNextTrial();
		}

		void prepareNewTest() {
			model.prepareNewTest(&testParameters);
		}

		void playNextTrial() {
			model.playNextTrial(&trialParameters);
		}

		void playCalibration() {
			model.playCalibration(&calibrationParameters);
		}

		void processAudioForSaving() {
			model.processAudioForSaving(&saveAudioParameters);
		}
		
		void setInMemoryReader(AudioFileReader &reader_) {
			audioFrameReaderFactory.setReader(std::make_shared<AudioFileInMemory>(reader_));
		}

		void setFullSimulationForTest() noexcept {
			setFullSimulation(testParameters.processing);
		}

		void setFullSimulation(RefactoredModel::ProcessingParameters &p) noexcept {
			p.usingHearingAidSimulation = true;
			p.usingSpatialization = true;
		}

		void setHearingAidSimulationOnlyForTest() noexcept {
			setHearingAidSimulationOnly(testParameters.processing);
		}
		
		void setHearingAidSimulationOnly(RefactoredModel::ProcessingParameters &p) noexcept {
			p.usingHearingAidSimulation = true;
			p.usingSpatialization = false;
		}

		void setSpatializationOnlyForTest() noexcept {
			setSpatializationOnly(testParameters.processing);
		}
		
		void setSpatializationOnly(RefactoredModel::ProcessingParameters &p) noexcept {
			p.usingHearingAidSimulation = false;
			p.usingSpatialization = true;
		}

		void setNoSimulationForTest() noexcept {
			setNoSimulation(testParameters.processing);
		}
		
		void setNoSimulation(RefactoredModel::ProcessingParameters &p) noexcept {
			p.usingHearingAidSimulation = false;
			p.usingSpatialization = false;
		}

		void setFullSimulationForCalibration() noexcept {
			setFullSimulation(calibrationParameters.processing);
		}

		void setFullSimulationForSaving() noexcept {
			setFullSimulation(saveAudioParameters.processing);
		}
		
		void setHearingAidSimulationOnlyForCalibration() noexcept {
			setHearingAidSimulationOnly(calibrationParameters.processing);
		}
		
		void setHearingAidSimulationOnlyForSaving() noexcept {
			setHearingAidSimulationOnly(saveAudioParameters.processing);
		}
		
		void setSpatializationOnlyForCalibration() noexcept {
			setSpatializationOnly(calibrationParameters.processing);
		}
		
		void setSpatializationOnlyForSaving() noexcept {
			setSpatializationOnly(saveAudioParameters.processing);
		}
		
		void setNoSimulationForCalibration() noexcept {
			setNoSimulation(calibrationParameters.processing);
		}
		
		void setNoSimulationForSaving() noexcept {
			setNoSimulation(saveAudioParameters.processing);
		}
		
		void processWhenPlayerPlays(gsl::span<channel_type> channels) {
			callWhenPlayerPlays([=]() { processAudioLoaderProcessor(channels); });
		}

		void callWhenPlayerPlays(std::function<void(void)> f) {
			audioPlayer.callOnPlay([=]() { f(); });
		}

		void processAudioLoaderProcessor(gsl::span<channel_type> channels) {
			audioLoader.audioFrameProcessor()->process(channels);
		}

		void assertAudioLoaderHasNotBeenModified() {
			assertTrue(audioLoader.log().isEmpty());
		}

		void assertFullSimulationNotMade() {
			assertTrue(simulationFactory.fullSimulationHearingAid().empty());
		}

		void assertHearingAidSimulationOnlyNotMade() {
			assertTrue(simulationFactory.hearingAidSimulation().empty());
		}

		void assertSpatializationOnlyNotMade() {
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

		void assertCalibrationFactoryReceivesAudioFrameReader() noexcept {
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

		void assertHearingAidSimulationOnlyYieldsCompressionParametersMatchingAfterCall(
			RefactoredModel::ProcessingParameters &processing,
			std::function<void(void)> f
		) {
			setHearingAidSimulationOnly(processing);
			assertHearingAidCompressionParametersMatchAfterCall(
				processing,
				simulationFactory.hearingAidSimulation(),
				f
			);
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

		void assertFullSimulationYieldsCompressionParametersMatchingAfterCall(
			RefactoredModel::ProcessingParameters &processing,
			std::function<void(void)> f
		) {
			setFullSimulation(processing);
			assertHearingAidCompressionParametersMatchAfterCall(
				processing,
				simulationFactory.fullSimulationHearingAid(),
				f
			);
		}

		void assertHearingAidSimulationOnlyYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			RefactoredModel::ProcessingParameters &processing,
			std::function<void(void)> f
		) {
			setHearingAidSimulationOnly(processing);
			assertHearingAidSimulationSampleRateMatchesAudioReaderAfterCall(
				simulationFactory.hearingAidSimulation(),
				f
			);
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

		void assertFullSimulationYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			RefactoredModel::ProcessingParameters &processing,
			std::function<void(void)> f
		) {
			setFullSimulation(processing);
			assertHearingAidSimulationSampleRateMatchesAudioReaderAfterCall(
				simulationFactory.fullSimulationHearingAid(),
				f
			);
		}

		void assertHearingAidSimulationFullScaleLevelMatchesAfterCall(
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::HearingAidSimulation> &hearingAid,
			std::function<void(void)> f
		) {
			f();
			assertEqual(
				RefactoredModel::fullScaleLevel_dB_Spl, 
				hearingAid.at(0).fullScaleLevel_dB_Spl
			);
			assertEqual(
				RefactoredModel::fullScaleLevel_dB_Spl, 
				hearingAid.at(1).fullScaleLevel_dB_Spl
			);
		}

		void assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			PoppableVector<std::shared_ptr<SignalProcessor>> &processors,
			std::function<void(void)> f
		) {
			std::vector<std::shared_ptr<SignalProcessor>> simulation = {
				std::make_shared<MultipliesSamplesBy>(2.0f),
				std::make_shared<MultipliesSamplesBy>(3.0f)
			};
			processors.set(simulation);

			buffer_type left = { 5 };
			buffer_type right = { 7 };
			std::vector<channel_type> channels = { left, right };
			processWhenPlayerPlays(channels);

			f();
			
			assertEqual({ 5 * 2 }, left);
			assertEqual({ 7 * 3 }, right);
		}

		void assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::Spatialization> &spatialization,
			std::function<void(void)> f
		) {
			BrirReader::BinauralRoomImpulseResponse brir;
			brir.left = { 1, 2 };
			brir.right = { 3, 4 };
			brirReader.setBrir(brir);
			f();
			assertEqual({ 1, 2, }, spatialization.at(0).filterCoefficients);
			assertEqual({ 3, 4, }, spatialization.at(1).filterCoefficients);
		}

		void assertPrescriptionReaderContainsFilePathsAfterCall(
			RefactoredModel::ProcessingParameters &processing,
			std::function<void(void)> f
		) {
			processing.leftDslPrescriptionFilePath = "a";
			processing.rightDslPrescriptionFilePath = "b";
			f();
			assertTrue(prescriptionReader.filePaths().contains("a"));
			assertTrue(prescriptionReader.filePaths().contains("b"));
		}

		void assertPrescriptionReaderDidNotReadAnything() {
			assertTrue(prescriptionReader.filePaths().empty());
		}

		void assertBrirReaderReceivesFilePathAfterCall(
			RefactoredModel::ProcessingParameters &processing,
			std::function<void(void)> f
		) {
			processing.brirFilePath = "a";
			f();
			assertEqual("a", brirReader.filePath());
		}

		void assertAudioReaderFactoryReceivesFilePathAfterCall(
			std::string &filePath,
			std::function<void(void)> f
		) {
			filePath = "a";
			f();
			assertEqual("a", audioFrameReaderFactory.filePath());
		}
		
		void assertBrirReaderDidNotReadAnything() {
			assertFalse(brirReader.readCalled());
		}

		void assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(
			std::function<void(void)> f
		) {
			callWhenPlayerPlays([=]() { assertAudioFrameReaderPassedToLoader(); });
			f();
		}

		void assertAudioFrameReaderPassedToLoader() noexcept {
			EXPECT_EQ(audioFrameReader, audioLoader.audioFrameReader());
		}

		void assertPlayerPreparedPriorToPlaying() {
			assertEqual("prepareToPlay play ", audioPlayer.log());
		}

		void assertNoHearingAidSimulationYieldsNoSuchSimulationMadeAfterCall(
			RefactoredModel::ProcessingParameters &p,
			std::function<void(void)> f
		) {
			p.usingHearingAidSimulation = false;
			f();
			assertFullSimulationNotMade();
			assertHearingAidSimulationOnlyNotMade();
		}

		void assertNoSpatializationYieldsNoSuchSimulationMadeAfterCall(
			RefactoredModel::ProcessingParameters &p,
			std::function<void(void)> f
		) {
			p.usingSpatialization = false;
			f();
			assertFullSimulationNotMade();
			assertSpatializationOnlyNotMade();
		}
	};

	TEST_F(
		RefactoredModelTests,
		prepareNewTestInitializesStimulusList
	) {
		testParameters.audioDirectory = "a";
		prepareNewTest();
		assertEqual("a", list.directory());
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestInitializesDocumenter
	) {
		testParameters.testFilePath = "a";
		prepareNewTest();
		assertEqual("a", documenter.filePath());
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestDocumentsTestParameters
	) {
		prepareNewTest();
		EXPECT_EQ(&testParameters, documenter.documentedTestParameters());
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestDocumentsTestParametersAfterInitializing
	) {
		prepareNewTest();
		assertTrue(documenter.log().beginsWith("initialize"));
	}

	TEST_F(
		RefactoredModelTests,
		playTrialPassesNextStimulusToFactory
	) {
		list.setContents({ "a", "b", "c" });
		playFirstTrialOfNewTest();
		assertEqual("a", audioFrameReaderFactory.filePath());
		playNextTrial();
		assertEqual("b", audioFrameReaderFactory.filePath());
		playNextTrial();
		assertEqual("c", audioFrameReaderFactory.filePath());
	}

	TEST_F(
		RefactoredModelTests,
		playNextTrialDocumentsTrial
	) {
		list.setContents({ "a", "b", "c" });
		trialParameters.level_dB_Spl = 1.1;
		playFirstTrialOfNewTest();
		assertEqual("a", documenter.documentedTrialParameters().stimulus);
		assertEqual(1.1, documenter.documentedTrialParameters().level_dB_Spl);
		trialParameters.level_dB_Spl = 2.2;
		playNextTrial();
		assertEqual("b", documenter.documentedTrialParameters().stimulus);
		assertEqual(2.2, documenter.documentedTrialParameters().level_dB_Spl);
	}

	TEST_F(RefactoredModelTests, constructorAssignsAudioLoaderToPlayer) {
		EXPECT_EQ(&audioLoader, audioPlayer.audioLoader());
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		testParameters.processing.usingHearingAidSimulation = true;
		assertPrescriptionReaderContainsFilePathsAfterCall(
			testParameters.processing,
			[=]() { prepareNewTest(); }
		);

	}

	TEST_F(
		RefactoredModelTests,
		playCalibrationPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		calibrationParameters.processing.usingHearingAidSimulation = true;
		assertPrescriptionReaderContainsFilePathsAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests,
		processAudioForSavingPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		saveAudioParameters.processing.usingHearingAidSimulation = true;
		assertPrescriptionReaderContainsFilePathsAfterCall(
			saveAudioParameters.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		testParameters.processing.usingHearingAidSimulation = false;
		prepareNewTest();
		assertPrescriptionReaderDidNotReadAnything();
	}

	TEST_F(
		RefactoredModelTests,
		playCalibrationDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		calibrationParameters.processing.usingHearingAidSimulation = false;
		playCalibration();
		assertPrescriptionReaderDidNotReadAnything();
	}

	TEST_F(
		RefactoredModelTests,
		processAudioForSavingDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		saveAudioParameters.processing.usingHearingAidSimulation = false;
		processAudioForSaving();
		assertPrescriptionReaderDidNotReadAnything();
	}

	TEST_F(
		RefactoredModelTests,
		prepareNewTestPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		testParameters.processing.usingSpatialization = true;
		assertBrirReaderReceivesFilePathAfterCall(
			testParameters.processing,
			[=]() { prepareNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests,
		playCalibrationPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		calibrationParameters.processing.usingSpatialization = true;
		assertBrirReaderReceivesFilePathAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests,
		processAudioForSavingPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		saveAudioParameters.processing.usingSpatialization = true;
		assertBrirReaderReceivesFilePathAfterCall(
			saveAudioParameters.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(RefactoredModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
		testParameters.processing.usingSpatialization = false;
		prepareNewTest();
		assertBrirReaderDidNotReadAnything();
	}

	TEST_F(RefactoredModelTests, playCalibrationDoesNotReadBrirWhenNotUsingSpatialization) {
		calibrationParameters.processing.usingSpatialization = false;
		playCalibration();
		assertBrirReaderDidNotReadAnything();
	}

	TEST_F(RefactoredModelTests, processAudioForSavingDoesNotReadBrirWhenNotUsingSpatialization) {
		saveAudioParameters.processing.usingSpatialization = false;
		processAudioForSaving();
		assertBrirReaderDidNotReadAnything();
	}

	TEST_F(RefactoredModelTests, playTrialPlaysPlayer) {
		playNextTrial();
		assertAudioPlayerHasBeenPlayed();
	}

	TEST_F(RefactoredModelTests, playCalibrationPlaysPlayer) {
		playCalibration();
		assertAudioPlayerHasBeenPlayed();
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesAudioFileToFactory) {
		assertAudioReaderFactoryReceivesFilePathAfterCall(
			calibrationParameters.audioFilePath,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, processAudioForSavingPassesAudioFileToFactory) {
		assertAudioReaderFactoryReceivesFilePathAfterCall(
			saveAudioParameters.inputAudioFilePath,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(
			[=]() { playNextTrial(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReaderAfterCall([=]() { playNextTrial(); });
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReaderAfterCall([=]() { playCalibration(); });
	}

	TEST_F(RefactoredModelTests, playTrialPassesAudioDeviceToPlayer) {
		trialParameters.audioDevice = "a";
		playNextTrial();
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
		playFirstTrialOfNewTest();
		assertCalibrationFactoryReceivesAudioFrameReader();
	}

	TEST_F(RefactoredModelTests, playCalibrationPassesAudioFrameReaderToCalibrationFactory) {
		playCalibration();
		assertCalibrationFactoryReceivesAudioFrameReader();
	}

	TEST_F(RefactoredModelTests, processAudioForSavingPassesAudioFrameReaderToCalibrationFactory) {
		processAudioForSaving();
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

	TEST_F(RefactoredModelTests, processAudioForSavingPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevelsAfterCall(
			saveAudioParameters.level_dB_Spl,
			[=]() { processAudioForSaving(); }
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

	TEST_F(RefactoredModelTests, processAudioForSavingComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForSaving();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.fullSimulationScale(),
			[=]() { processAudioForSaving(); }
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

	TEST_F(RefactoredModelTests, processAudioForSavingComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForSaving();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { processAudioForSaving(); }
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

	TEST_F(RefactoredModelTests, processAudioForSavingComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForSaving();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.spatializationScale(),
			[=]() { processAudioForSaving(); }
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

	TEST_F(RefactoredModelTests, processAudioForSavingComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForSaving();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.withoutSimulationScale(),
			[=]() { processAudioForSaving(); }
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
		processAudioForSavingPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForSaving();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			saveAudioParameters.processing, 
			simulationFactory.hearingAidSimulation(),
			[=]() { processAudioForSaving(); }
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
		processAudioForSavingPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForSaving();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			saveAudioParameters.processing, 
			simulationFactory.fullSimulationHearingAid(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMadeAfterCall(
			testParameters.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMadeAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		processAudioForSavingDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMadeAfterCall(
			saveAudioParameters.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMadeAfterCall(
			testParameters.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMadeAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		processAudioForSavingDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMadeAfterCall(
			saveAudioParameters.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsCompressionParametersMatchingAfterCall(
			testParameters.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsCompressionParametersMatchingAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		processAudioForSavingPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsCompressionParametersMatchingAfterCall(
			saveAudioParameters.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsCompressionParametersMatchingAfterCall(
			testParameters.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsCompressionParametersMatchingAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		processAudioForSavingPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsCompressionParametersMatchingAfterCall(
			saveAudioParameters.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			testParameters.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		processAudioForSavingPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			saveAudioParameters.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			testParameters.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			calibrationParameters.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		processAudioForSavingPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			saveAudioParameters.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForTest();
		assertHearingAidSimulationFullScaleLevelMatchesAfterCall(
			simulationFactory.hearingAidSimulation(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForCalibration();
		assertHearingAidSimulationFullScaleLevelMatchesAfterCall(
			simulationFactory.hearingAidSimulation(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesFullScaleLevelToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertHearingAidSimulationFullScaleLevelMatchesAfterCall(
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesFullScaleLevelToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertHearingAidSimulationFullScaleLevelMatchesAfterCall(
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialAssignsFullSimulationProcessorsToAudioLoader) {
		setFullSimulationForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.fullSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationAssignsFullSimulationProcessorsToAudioLoader) {
		setFullSimulationForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.fullSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialAssignsHearingAidSimulationProcessorsToAudioLoader) {
		setHearingAidSimulationOnlyForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.hearingAidSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationAssignsHearingAidSimulationProcessorsToAudioLoader) {
		setHearingAidSimulationOnlyForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.hearingAidSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialAssignsSpatializationProcessorsToAudioLoader) {
		setSpatializationOnlyForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.spatializationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationAssignsSpatializationProcessorsToAudioLoader) {
		setSpatializationOnlyForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.spatializationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialAssignsWithoutSimulationProcessorsToAudioLoader) {
		setNoSimulationForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.withoutSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(RefactoredModelTests, playCalibrationAssignsWithoutSimulationProcessorsToAudioLoader) {
		setNoSimulationForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.withoutSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesBrirToFactoryForSpatialization
	) {
		setSpatializationOnlyForTest();
		assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			simulationFactory.spatialization(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesBrirToFactoryForSpatialization
	) {
		setSpatializationOnlyForCalibration();
		assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			simulationFactory.spatialization(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playTrialPassesBrirToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			simulationFactory.fullSimulationSpatialization(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		RefactoredModelTests, 
		playCalibrationPassesBrirToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			simulationFactory.fullSimulationSpatialization(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(RefactoredModelTests, playTrialResetsAudioLoaderBeforePlaying) {
		callWhenPlayerPlays([=]() { assertTrue(audioLoader.log().contains("reset")); });
		playNextTrial();
	}

	TEST_F(RefactoredModelTests, playCalibrationResetsAudioLoaderBeforePlaying) {
		callWhenPlayerPlays([=]() { assertTrue(audioLoader.log().contains("reset")); });
		playCalibration();
	}

	TEST_F(RefactoredModelTests, playTrialPreparesPlayerBeforePlaying) {
		playNextTrial();
		assertPlayerPreparedPriorToPlaying();
	}

	TEST_F(RefactoredModelTests, playCalibrationPreparesPlayerBeforePlaying) {
		playCalibration();
		assertPlayerPreparedPriorToPlaying();
	}

	TEST_F(RefactoredModelTests, playTrialDoesNotAlterLoaderWhenPlayerPlaying) {
		audioPlayer.setPlaying();
		playNextTrial();
		assertAudioLoaderHasNotBeenModified();
	}

	TEST_F(RefactoredModelTests, playCalibrationDoesNotAlterLoaderWhenPlayerPlaying) {
		audioPlayer.setPlaying();
		playCalibration();
		assertAudioLoaderHasNotBeenModified();
	}

	TEST_F(RefactoredModelTests, audioDeviceDescriptionsReturnsDescriptionsFromPlayer) {
		audioPlayer.setAudioDeviceDescriptions({ "a", "b", "c" });
		assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
	}

	TEST_F(RefactoredModelTests, testCompleteWhenListEmpty) {
		list.setContents({});
		assertTrue(model.testComplete());
	}

	TEST_F(RefactoredModelTests, stopCalibrationStopsPlayer) {
		model.stopCalibration();
		assertTrue(audioPlayer.stopped());
	}

	class RefactoredModelFailureTests : public ::testing::Test {
	protected:
		RefactoredModel::TestParameters testParameters{};
		RefactoredModel::TrialParameters trialParameters{};
		RefactoredModel::CalibrationParameters calibrationParameters{};
		PrescriptionReaderStub defaultPrescriptionReader{};
		PrescriptionReader *prescriptionReader{ &defaultPrescriptionReader };
		BrirReaderStub defaultBrirReader{};
		BrirReader *brirReader{ &defaultBrirReader };
		FakeStimulusList defaultStimulusList{};
		StimulusList *stimulusList{ &defaultStimulusList };
		DocumenterStub defaultDocumenter{};
		Documenter *documenter{ &defaultDocumenter };
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
			auto model = constructModel();
			try {
				model.prepareNewTest(&testParameters);
				FAIL() << "Expected RefactoredModel::RequestFailure.";
			}
			catch (const RefactoredModel::RequestFailure & e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayTrialThrowsRequestFailure(std::string what) {
			auto model = constructModel();
			try {
				model.playNextTrial(&trialParameters);
				FAIL() << "Expected RefactoredModel::RequestFailure.";
			}
			catch (const RefactoredModel::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayCalibrationThrowsRequestFailure(std::string what) {
			auto model = constructModel();
			try {
				model.playCalibration(&calibrationParameters);
				FAIL() << "Expected RefactoredModel::RequestFailure.";
			}
			catch (const RefactoredModel::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void prepareNewTestIgnoringFailure() {
			auto model = constructModel();
			try {
				model.prepareNewTest(&testParameters);
			}
			catch (const RefactoredModel::RequestFailure &) {
			}
		}

		void playTrialIgnoringFailure() {
			auto model = constructModel();
			try {
				model.playNextTrial(&trialParameters);
			}
			catch (const RefactoredModel::RequestFailure &) {
			}
		}

		RefactoredModel constructModel() {
			return
			{
				stimulusList,
				documenter,
				audioPlayer,
				audioLoader,
				audioReaderFactory,
				prescriptionReader,
				brirReader,
				simulationFactory,
				calibrationFactory
			};
		}

		void setValidSizesForTest() {
			testParameters.processing.chunkSize = 1;
			testParameters.processing.windowSize = 1;
		}

		void setValidSizesForCalibration() {
			calibrationParameters.processing.chunkSize = 1;
			calibrationParameters.processing.windowSize = 1;
		}
	};

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsInitializationFailureWhenDocumenterFailsToInitialize
	) {
		InitializationFailingDocumenter failing;
		documenter = &failing;
		failing.setErrorMessage("error.");
		assertPreparingNewTestThrowsRequestFailure("error.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		testParameters.processing.usingHearingAidSimulation = true;
		setValidSizesForTest();
		testParameters.processing.leftDslPrescriptionFilePath = "a";
		testParameters.processing.rightDslPrescriptionFilePath = "a";
		assertPreparingNewTestThrowsRequestFailure("Prescription 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		calibrationParameters.processing.usingHearingAidSimulation = true;
		setValidSizesForCalibration();
		calibrationParameters.processing.leftDslPrescriptionFilePath = "a";
		calibrationParameters.processing.rightDslPrescriptionFilePath = "a";
		assertPlayCalibrationThrowsRequestFailure("Prescription 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotDocumentWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		testParameters.processing.usingHearingAidSimulation = true;
		prepareNewTestIgnoringFailure();
		assertTrue(defaultDocumenter.log().isEmpty());
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		testParameters.processing.usingSpatialization = true;
		testParameters.processing.brirFilePath = "a";
		assertPreparingNewTestThrowsRequestFailure("BRIR 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		calibrationParameters.processing.usingSpatialization = true;
		calibrationParameters.processing.brirFilePath = "a";
		assertPlayCalibrationThrowsRequestFailure("BRIR 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotDocumentWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		testParameters.processing.usingSpatialization = true;
		prepareNewTestIgnoringFailure();
		assertTrue(defaultDocumenter.log().isEmpty());
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
		playCalibrationThrowsRequestFailureWhenChunkSizeIsNotPowerOfTwo
	) {
		calibrationParameters.processing.usingHearingAidSimulation = true;
		calibrationParameters.processing.chunkSize = 0;
		calibrationParameters.processing.windowSize = 1;
		assertPlayCalibrationThrowsRequestFailure(
			"Both the chunk size and window size must be powers of two; 0 is not a power of two."
		);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenWindowSizeIsNotPowerOfTwo
	) {
		calibrationParameters.processing.usingHearingAidSimulation = true;
		calibrationParameters.processing.chunkSize = 2;
		calibrationParameters.processing.windowSize = 3;
		assertPlayCalibrationThrowsRequestFailure(
			"Both the chunk size and window size must be powers of two; 3 is not a power of two."
		);
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenStimulusListFails
	) {
		FailsToInitializeStimulusList failing;
		failing.setErrorMessage("error.");
		stimulusList = &failing;
		assertPreparingNewTestThrowsRequestFailure("error.");
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

	TEST_F(RefactoredModelFailureTests, playTrialDoesNotAdvanceStimulusWhenPlayerFails) {
		PreparationFailingAudioPlayer failing;
		audioPlayer = &failing;
		defaultStimulusList.setContents({ "a", "b", "c" });
		playTrialIgnoringFailure();
		assertEqual("a", defaultStimulusList.next());
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