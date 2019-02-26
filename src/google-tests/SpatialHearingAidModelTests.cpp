#include "FilterbankCompressorSpy.h"
#include "FakeAudioProcessingLoader.h"
#include "AudioFrameReaderStub.h"
#include "AudioProcessingLoaderStub.h"
#include "PrescriptionReaderStub.h"
#include "BrirReaderStub.h"
#include "FakeAudioFile.h"
#include "SignalProcessorStub.h"
#include "AudioPlayerStub.h"
#include "FakeStimulusList.h"
#include "DocumenterStub.h"
#include "CalibrationComputerStub.h"
#include "SpatializedHearingAidSimulationFactoryStub.h"
#include "AudioFrameWriterStub.h"
#include "assert-utility.h"
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <spatialized-hearing-aid-simulation/SpatialHearingAidModel.h>
#include <gtest/gtest.h>

namespace {
	class SpatialHearingAidModelTests : public ::testing::Test {
	protected:
		struct ProcessingUseCase {
			SpatialHearingAidModel::SignalProcessing &processing;
			std::function<void()> request;
		};

		using channel_type = AudioFrameProcessor::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;

		SpatialHearingAidModel::Testing testing{};
		SpatialHearingAidModel::Trial trial{};
		SpatialHearingAidModel::Calibration calibration{};
		SpatialHearingAidModel::SavingAudio savingAudio{};
		PrescriptionReaderStub prescriptionReader{};
		BrirReaderStub brirReader{};
		FakeStimulusList stimulusList{};
		DocumenterStub documenter{};
		std::shared_ptr<AudioFrameReaderStub> audioFrameReader
			= std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory audioFrameReaderFactory{ audioFrameReader };
		std::shared_ptr<AudioFrameWriterStub> audioFrameWriter
			= std::make_shared<AudioFrameWriterStub>();
		AudioFrameWriterStubFactory audioFrameWriterFactory{ audioFrameWriter };
		AudioPlayerStub audioPlayer{};
		std::shared_ptr<AudioProcessingLoaderStub> audioLoader =
			std::make_shared<AudioProcessingLoaderStub>();
		AudioProcessingLoaderStubFactory audioLoaderFactory{ audioLoader };
		SpatializedHearingAidSimulationFactoryStub simulationFactory{};
		std::shared_ptr<CalibrationComputerStub> calibrationComputer =
			std::make_shared<CalibrationComputerStub>();
		CalibrationComputerStubFactory calibrationComputerFactory{ calibrationComputer };
		SpatialHearingAidModel model{
			&stimulusList,
			&documenter,
			&audioPlayer,
			&audioLoaderFactory,
			&audioFrameReaderFactory,
			&audioFrameWriterFactory,
			&prescriptionReader,
			&brirReader,
			&simulationFactory,
			&calibrationComputerFactory
		};
		ProcessingUseCase preparingNewTest{
			testing.processing,
			[=]() { prepareNewTest(); }
		};
		ProcessingUseCase playingFirstTrialOfNewTest{
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		};
		ProcessingUseCase playingCalibration{
			calibration.processing,
			[=]() { playCalibration(); }
		};
		ProcessingUseCase processingAudioForSaving{
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		};

		SpatialHearingAidModelTests() {
			setValidDefaults();
		}

		void setValidDefaults() {
			BrirReader::BinauralRoomImpulseResponse brir;
			brir.left = { 0 };
			brir.right = { 0 };
			brirReader.setBrir(brir);
			setValidProcessingSizes(testing.processing);
			setValidProcessingSizes(calibration.processing);
			setValidProcessingSizes(savingAudio.processing);
			audioLoader->setComplete();
		}

		void setValidProcessingSizes(SpatialHearingAidModel::SignalProcessing &p) noexcept {
			p.chunkSize = 1;
			p.windowSize = 1;
		}

		void playFirstTrialOfNewTest() {
			prepareNewTest();
			playNextTrial();
		}

		void prepareNewTest() {
			model.prepareNewTest(&testing);
		}

		void playNextTrial() {
			model.playNextTrial(&trial);
		}

		void playCalibration() {
			model.playCalibration(&calibration);
		}

		void processAudioForSaving() {
			model.processAudioForSaving(&savingAudio);
		}
		
		void setInMemoryReader(AudioFileReader &reader_) {
			audioFrameReaderFactory.setReader(std::make_shared<AudioFileInMemory>(reader_));
		}

		void setFullSimulationForTest() noexcept {
			setFullSimulation(testing.processing);
		}

		void setFullSimulation(SpatialHearingAidModel::SignalProcessing &p) noexcept {
			p.usingHearingAidSimulation = true;
			p.usingSpatialization = true;
		}

		void setHearingAidSimulationOnlyForTest() noexcept {
			setHearingAidSimulationOnly(testing.processing);
		}
		
		void setHearingAidSimulationOnly(SpatialHearingAidModel::SignalProcessing &p) noexcept {
			p.usingHearingAidSimulation = true;
			p.usingSpatialization = false;
		}

		void setSpatializationOnlyForTest() noexcept {
			setSpatializationOnly(testing.processing);
		}
		
		void setSpatializationOnly(SpatialHearingAidModel::SignalProcessing &p) noexcept {
			p.usingHearingAidSimulation = false;
			p.usingSpatialization = true;
		}

		void setNoSimulationForTest() noexcept {
			setNoSimulation(testing.processing);
		}
		
		void setNoSimulation(SpatialHearingAidModel::SignalProcessing &p) noexcept {
			p.usingHearingAidSimulation = false;
			p.usingSpatialization = false;
		}

		void setFullSimulationForCalibration() noexcept {
			setFullSimulation(calibration.processing);
		}

		void setFullSimulationForSaving() noexcept {
			setFullSimulation(savingAudio.processing);
		}
		
		void setHearingAidSimulationOnlyForCalibration() noexcept {
			setHearingAidSimulationOnly(calibration.processing);
		}
		
		void setHearingAidSimulationOnlyForSaving() noexcept {
			setHearingAidSimulationOnly(savingAudio.processing);
		}
		
		void setSpatializationOnlyForCalibration() noexcept {
			setSpatializationOnly(calibration.processing);
		}
		
		void setSpatializationOnlyForSaving() noexcept {
			setSpatializationOnly(savingAudio.processing);
		}
		
		void setNoSimulationForCalibration() noexcept {
			setNoSimulation(calibration.processing);
		}
		
		void setNoSimulationForSaving() noexcept {
			setNoSimulation(savingAudio.processing);
		}
		
		void processWhenPlayerPlays(gsl::span<channel_type> channels) {
			callWhenPlayerPlays([=]() { processAudioLoaderProcessor(channels); });
		}

		void callWhenPlayerPlays(std::function<void(void)> f) {
			audioPlayer.callOnPlay([=]() { f(); });
		}

		void processAudioLoaderProcessor(gsl::span<channel_type> channels) {
			audioLoaderFactory.audioFrameProcessor()->process(channels);
		}

		void assertAudioLoaderHasNotBeenModified() {
			assertTrue(audioLoader->log().isEmpty());
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

		void assertAudioPlayerParametersMatchAudioFrameReader(std::function<void(void)> f) {
			audioFrameReader->setChannels(1);
			audioFrameReader->setSampleRate(2);
			f();
			assertEqual(1, audioPlayer.preparation().channels);
			assertEqual(2, audioPlayer.preparation().sampleRate);
		}

		void assertFramesPerBufferMatchesChunkSizeWhenUsingHearingAidSimulation(ProcessingUseCase useCase) {
			useCase.processing.usingHearingAidSimulation = true;
			useCase.processing.chunkSize = 1;
			useCase.request();
			assertEqual(1, audioPlayer.preparation().framesPerBuffer);
		}

		void assertFramesPerBufferMatchesDefaultWhenNotUsingHearingAidSimulation(ProcessingUseCase useCase) {
			useCase.processing.usingHearingAidSimulation = false;
			useCase.request();
			assertEqual(
				SpatialHearingAidModel::defaultFramesPerBuffer, 
				audioPlayer.preparation().framesPerBuffer
			);
		}

		void assertCalibrationComputerFactoryReceivesAudioFrameReader(ProcessingUseCase useCase) noexcept {
			useCase.request();
			EXPECT_EQ(audioFrameReader.get(), calibrationComputerFactory.reader());
		}

		void assertCalibrationDigitalLevels(double &level, std::function<void(void)> f) {
			audioFrameReader->setChannels(2);
			level = 65;
			f();
			assertEqual(65 - SpatialHearingAidModel::fullScaleLevel_dB_Spl, calibrationComputer->levels().at(0));
			assertEqual(65 - SpatialHearingAidModel::fullScaleLevel_dB_Spl, calibrationComputer->levels().at(1));
		}

		void assertScalarsMatchCalibration(
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

		void assertSimulationPrescriptionsMatchPrescriptionReader(
			ProcessingUseCase useCase,
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::HearingAidSimulation> &hearingAid
		) {
			PrescriptionReader::Dsl left;
			left.compressionRatios = { 1 };
			left.crossFrequenciesHz = { 2 };
			left.kneepointGains_dB = { 3 };
			left.kneepoints_dBSpl = { 4 };
			left.broadbandOutputLimitingThresholds_dBSpl = { 5 };
			left.channels = 6;
			useCase.processing.leftDslPrescriptionFilePath = "leftFilePath";
			prescriptionReader.addPrescription("leftFilePath", left);

			PrescriptionReader::Dsl right;
			right.compressionRatios = { 7 };
			right.crossFrequenciesHz = { 8 };
			right.kneepointGains_dB = { 9 };
			right.kneepoints_dBSpl = { 10 };
			right.broadbandOutputLimitingThresholds_dBSpl = { 11 };
			right.channels = 12;
			useCase.processing.rightDslPrescriptionFilePath = "rightFilePath";
			prescriptionReader.addPrescription("rightFilePath", right);

			useCase.request();

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

		void assertSimulationFactoryReceivesCompressionParametersWhenUsingOnlyHearingAidSimulation(
			ProcessingUseCase useCase
		) {
			setHearingAidSimulationOnly(useCase.processing);
			assertSimulationFactoryReceivesCompressionParameters(
				useCase,
				simulationFactory.hearingAidSimulation()
			);
		}

		void assertSimulationFactoryReceivesCompressionParameters(
			ProcessingUseCase useCase,
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::HearingAidSimulation> &hearingAid
		) {
			useCase.processing.attack_ms = 1;
			useCase.processing.release_ms = 2;
			useCase.processing.chunkSize = 4;
			useCase.processing.windowSize = 8;
			
			useCase.request();

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

		void assertSimulationFactoryReceivesCompressionParametersWhenUsingFullSimulation(
			ProcessingUseCase useCase
		) {
			setFullSimulation(useCase.processing);
			assertSimulationFactoryReceivesCompressionParameters(
				useCase,
				simulationFactory.fullSimulationHearingAid()
			);
		}

		void assertSimulationFactoryReceivesSampleRateWhenUsingOnlyHearingAidSimulation(
			ProcessingUseCase useCase
		) {
			setHearingAidSimulationOnly(useCase.processing);
			assertSimulationFactoryReceivesSampleRate(
				simulationFactory.hearingAidSimulation(),
				useCase.request
			);
		}

		void assertSimulationFactoryReceivesSampleRate(
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::HearingAidSimulation> &hearingAid,
			std::function<void(void)> f
		) {
			audioFrameReader->setSampleRate(1);
			f();
			assertEqual(1, hearingAid.at(0).sampleRate);
			assertEqual(1, hearingAid.at(1).sampleRate);
		}

		void assertSimulationFactoryReceivesSampleRateWhenUsingFullSimulation(
			ProcessingUseCase useCase
		) {
			setFullSimulation(useCase.processing);
			assertSimulationFactoryReceivesSampleRate(
				simulationFactory.fullSimulationHearingAid(),
				useCase.request
			);
		}

		void assertHearingAidSimulationOnlyYieldsFullScaleLevelMatching(
			ProcessingUseCase useCase
		) {
			setHearingAidSimulationOnly(useCase.processing);
			assertHearingAidSimulationFullScaleLevelMatches(
				simulationFactory.hearingAidSimulation(),
				useCase.request
			);
		}

		void assertHearingAidSimulationFullScaleLevelMatches(
			const ArgumentCollection<
				ISpatializedHearingAidSimulationFactory::HearingAidSimulation> &hearingAid,
			std::function<void(void)> f
		) {
			f();
			assertEqual(
				SpatialHearingAidModel::fullScaleLevel_dB_Spl, 
				hearingAid.at(0).fullScaleLevel_dB_Spl
			);
			assertEqual(
				SpatialHearingAidModel::fullScaleLevel_dB_Spl, 
				hearingAid.at(1).fullScaleLevel_dB_Spl
			);
		}

		void assertAudioLoaderAppliesSimulationWhenPlayerPlays(
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

		void assertSpatializationFilterCoefficientsMatchBrir(
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

		void assertPrescriptionReaderReceivesFilePathsWhenUsingHearingAidSimulation(
			ProcessingUseCase useCase
		) {
			useCase.processing.usingHearingAidSimulation = true;
			useCase.processing.leftDslPrescriptionFilePath = "a";
			useCase.processing.rightDslPrescriptionFilePath = "b";
			useCase.request();
			assertTrue(prescriptionReader.filePaths().contains("a"));
			assertTrue(prescriptionReader.filePaths().contains("b"));
		}

		void assertPrescriptionReaderDoesNotReadWhenNotUsingHearingAidSimulation(
			ProcessingUseCase useCase
		) {
			useCase.processing.usingHearingAidSimulation = false;
			useCase.request();
			assertTrue(prescriptionReader.filePaths().empty());
		}

		void assertBrirReaderReceivesFilePathWhenUsingSpatialization(
			ProcessingUseCase useCase
		) {
			useCase.processing.usingSpatialization = true;
			useCase.processing.brirFilePath = "a";
			useCase.request();
			assertEqual("a", brirReader.filePath());
		}

		void assertAudioReaderFactoryReceivesFilePath(
			std::string &filePath,
			std::function<void(void)> f
		) {
			filePath = "a";
			f();
			assertEqual("a", audioFrameReaderFactory.filePath());
		}
		
		void assertBrirReaderDoesNotReadWhenNotUsingSpatialization(
			ProcessingUseCase useCase
		) {
			useCase.processing.usingSpatialization = false;
			useCase.request();
			assertFalse(brirReader.readCalled());
		}

		void assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(
			std::function<void(void)> f
		) {
			callWhenPlayerPlays([=]() { assertAudioFrameReaderPassedToLoaderFactory(); });
			f();
		}

		void assertAudioFrameReaderPassedToLoaderFactory() noexcept {
			EXPECT_EQ(audioFrameReader, audioLoaderFactory.audioFrameReader());
		}

		void assertPlayerPreparedPriorToPlaying() {
			assertEqual("prepareToPlay play ", audioPlayer.log());
		}

		void assertNoHearingAidSimulationYieldsNoSuchSimulationMade(
			ProcessingUseCase useCase
		) {
			useCase.processing.usingHearingAidSimulation = false;
			useCase.request();
			assertFullSimulationNotMade();
			assertHearingAidSimulationOnlyNotMade();
		}

		void assertNoSpatializationYieldsNoSuchSimulationMade(
			ProcessingUseCase useCase
		) {
			useCase.processing.usingSpatialization = false;
			useCase.request();
			assertFullSimulationNotMade();
			assertSpatializationOnlyNotMade();
		}
	};

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestInitializesStimulusList
	) {
		testing.audioDirectory = "a";
		prepareNewTest();
		assertEqual("a", stimulusList.directory());
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestInitializesDocumenter
	) {
		testing.testFilePath = "a";
		prepareNewTest();
		assertEqual("a", documenter.filePath());
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestDocumentsTestParameters
	) {
		prepareNewTest();
		EXPECT_EQ(&testing, documenter.documentedTestParameters());
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestDocumentsTestParametersAfterInitializing
	) {
		prepareNewTest();
		assertTrue(documenter.log().beginsWith("initialize"));
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playTrialPassesNextStimulusToFactory
	) {
		stimulusList.setContents({ "a", "b", "c" });
		playFirstTrialOfNewTest();
		assertEqual("a", audioFrameReaderFactory.filePath());
		playNextTrial();
		assertEqual("b", audioFrameReaderFactory.filePath());
		playNextTrial();
		assertEqual("c", audioFrameReaderFactory.filePath());
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playNextTrialDocumentsTrial
	) {
		stimulusList.setContents({ "a", "b", "c" });
		trial.level_dB_Spl = 1.1;
		playFirstTrialOfNewTest();
		assertEqual("a", documenter.documentedTrialParameters().stimulus);
		assertEqual(1.1, documenter.documentedTrialParameters().level_dB_Spl);
		trial.level_dB_Spl = 2.2;
		playNextTrial();
		assertEqual("b", documenter.documentedTrialParameters().stimulus);
		assertEqual(2.2, documenter.documentedTrialParameters().level_dB_Spl);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		assertPrescriptionReaderReceivesFilePathsWhenUsingHearingAidSimulation(preparingNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		assertPrescriptionReaderReceivesFilePathsWhenUsingHearingAidSimulation(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		assertPrescriptionReaderReceivesFilePathsWhenUsingHearingAidSimulation(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		assertPrescriptionReaderDoesNotReadWhenNotUsingHearingAidSimulation(preparingNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		assertPrescriptionReaderDoesNotReadWhenNotUsingHearingAidSimulation(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		assertPrescriptionReaderDoesNotReadWhenNotUsingHearingAidSimulation(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		assertBrirReaderReceivesFilePathWhenUsingSpatialization(preparingNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		assertBrirReaderReceivesFilePathWhenUsingSpatialization(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		assertBrirReaderReceivesFilePathWhenUsingSpatialization(processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
		assertBrirReaderDoesNotReadWhenNotUsingSpatialization(preparingNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationDoesNotReadBrirWhenNotUsingSpatialization) {
		assertBrirReaderDoesNotReadWhenNotUsingSpatialization(playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingDoesNotReadBrirWhenNotUsingSpatialization) {
		assertBrirReaderDoesNotReadWhenNotUsingSpatialization(processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPlaysPlayer) {
		playNextTrial();
		assertAudioPlayerHasBeenPlayed();
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPlaysPlayer) {
		playCalibration();
		assertAudioPlayerHasBeenPlayed();
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioFileToFactory) {
		assertAudioReaderFactoryReceivesFilePath(
			calibration.audioFilePath,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesAudioFileToFactory) {
		assertAudioReaderFactoryReceivesFilePath(
			savingAudio.inputAudioFilePath,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(
			[=]() { playNextTrial(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReader([=]() { playNextTrial(); });
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReader([=]() { playCalibration(); });
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesAudioDeviceToPlayer) {
		trial.audioDevice = "a";
		playNextTrial();
		assertEqual("a", audioPlayer.preparation().audioDevice);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioDeviceToPlayer) {
		calibration.audioDevice = "a";
		playCalibration();
		assertEqual("a", audioPlayer.preparation().audioDevice);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation
	) {
		assertFramesPerBufferMatchesChunkSizeWhenUsingHearingAidSimulation(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation
	) {
		assertFramesPerBufferMatchesChunkSizeWhenUsingHearingAidSimulation(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playTrialUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		assertFramesPerBufferMatchesDefaultWhenNotUsingHearingAidSimulation(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		assertFramesPerBufferMatchesDefaultWhenNotUsingHearingAidSimulation(playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesAudioFrameReaderToCalibrationFactory) {
		assertCalibrationComputerFactoryReceivesAudioFrameReader(playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioFrameReaderToCalibrationFactory) {
		assertCalibrationComputerFactoryReceivesAudioFrameReader(playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesAudioFrameReaderToCalibrationFactory) {
		assertCalibrationComputerFactoryReceivesAudioFrameReader(processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevels(
			trial.level_dB_Spl,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevels(
			calibration.level_dB_Spl,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevels(
			savingAudio.level_dB_Spl,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForTest();
		assertScalarsMatchCalibration(
			simulationFactory.fullSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForCalibration();
		assertScalarsMatchCalibration(
			simulationFactory.fullSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForSaving();
		assertScalarsMatchCalibration(
			simulationFactory.fullSimulationScale(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForTest();
		assertScalarsMatchCalibration(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForCalibration();
		assertScalarsMatchCalibration(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForSaving();
		assertScalarsMatchCalibration(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForTest();
		assertScalarsMatchCalibration(
			simulationFactory.spatializationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForCalibration();
		assertScalarsMatchCalibration(
			simulationFactory.spatializationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForSaving();
		assertScalarsMatchCalibration(
			simulationFactory.spatializationScale(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForTest();
		assertScalarsMatchCalibration(
			simulationFactory.withoutSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForCalibration();
		assertScalarsMatchCalibration(
			simulationFactory.withoutSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForSaving();
		assertScalarsMatchCalibration(
			simulationFactory.withoutSimulationScale(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForTest();
		assertSimulationPrescriptionsMatchPrescriptionReader(
			playingFirstTrialOfNewTest,
			simulationFactory.hearingAidSimulation()
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForCalibration();
		assertSimulationPrescriptionsMatchPrescriptionReader(
			playingCalibration,
			simulationFactory.hearingAidSimulation()
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForSaving();
		assertSimulationPrescriptionsMatchPrescriptionReader(
			processingAudioForSaving,
			simulationFactory.hearingAidSimulation()
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertSimulationPrescriptionsMatchPrescriptionReader(
			playingFirstTrialOfNewTest, 
			simulationFactory.fullSimulationHearingAid()
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertSimulationPrescriptionsMatchPrescriptionReader(
			playingCalibration,
			simulationFactory.fullSimulationHearingAid()
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForSaving();
		assertSimulationPrescriptionsMatchPrescriptionReader(
			processingAudioForSaving,
			simulationFactory.fullSimulationHearingAid()
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMade(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMade(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMade(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMade(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMade(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMade(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingOnlyHearingAidSimulation(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingOnlyHearingAidSimulation(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingOnlyHearingAidSimulation(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingFullSimulation(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingFullSimulation(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingFullSimulation(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingOnlyHearingAidSimulation(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingOnlyHearingAidSimulation(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingOnlyHearingAidSimulation(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingFullSimulation(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingFullSimulation(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingFullSimulation(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsFullScaleLevelMatching(playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsFullScaleLevelMatching(playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsFullScaleLevelMatching(processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesFullScaleLevelToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertHearingAidSimulationFullScaleLevelMatches(
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesFullScaleLevelToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertHearingAidSimulationFullScaleLevelMatches(
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsFullSimulationProcessorsToAudioLoader) {
		setFullSimulationForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			simulationFactory.fullSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsFullSimulationProcessorsToAudioLoader) {
		setFullSimulationForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			simulationFactory.fullSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsHearingAidSimulationProcessorsToAudioLoader) {
		setHearingAidSimulationOnlyForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			simulationFactory.hearingAidSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsHearingAidSimulationProcessorsToAudioLoader) {
		setHearingAidSimulationOnlyForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			simulationFactory.hearingAidSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsSpatializationProcessorsToAudioLoader) {
		setSpatializationOnlyForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			simulationFactory.spatializationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsSpatializationProcessorsToAudioLoader) {
		setSpatializationOnlyForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			simulationFactory.spatializationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsWithoutSimulationProcessorsToAudioLoader) {
		setNoSimulationForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			simulationFactory.withoutSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsWithoutSimulationProcessorsToAudioLoader) {
		setNoSimulationForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			simulationFactory.withoutSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesBrirToFactoryForSpatialization
	) {
		setSpatializationOnlyForTest();
		assertSpatializationFilterCoefficientsMatchBrir(
			simulationFactory.spatialization(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesBrirToFactoryForSpatialization
	) {
		setSpatializationOnlyForCalibration();
		assertSpatializationFilterCoefficientsMatchBrir(
			simulationFactory.spatialization(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesBrirToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertSpatializationFilterCoefficientsMatchBrir(
			simulationFactory.fullSimulationSpatialization(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesBrirToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertSpatializationFilterCoefficientsMatchBrir(
			simulationFactory.fullSimulationSpatialization(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPreparesPlayerBeforePlaying) {
		playNextTrial();
		assertPlayerPreparedPriorToPlaying();
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPreparesPlayerBeforePlaying) {
		playCalibration();
		assertPlayerPreparedPriorToPlaying();
	}

	TEST_F(SpatialHearingAidModelTests, playTrialDoesNotAlterLoaderWhenPlayerPlaying) {
		audioPlayer.setPlaying();
		playNextTrial();
		assertAudioLoaderHasNotBeenModified();
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationDoesNotAlterLoaderWhenPlayerPlaying) {
		audioPlayer.setPlaying();
		playCalibration();
		assertAudioLoaderHasNotBeenModified();
	}

	TEST_F(SpatialHearingAidModelTests, audioDeviceDescriptionsReturnsDescriptionsFromPlayer) {
		audioPlayer.setAudioDeviceDescriptions({ "a", "b", "c" });
		assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
	}

	TEST_F(SpatialHearingAidModelTests, testCompleteWhenListEmpty) {
		stimulusList.setContents({});
		assertTrue(model.testComplete());
	}

	TEST_F(SpatialHearingAidModelTests, stopCalibrationStopsPlayer) {
		model.stopCalibration();
		assertTrue(audioPlayer.stopped());
	}

	TEST_F(SpatialHearingAidModelTests, saveAudioPassesFileToAudioWriterFactory) {
		model.saveAudio("a");
		assertEqual("a", audioFrameWriterFactory.filePath());
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesAudioFrameReaderToFactory) {
		processAudioForSaving();
		assertAudioFrameReaderPassedToLoaderFactory();
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingLoadsAudioUntilComplete) {
		std::shared_ptr<FakeAudioProcessingLoader> fakeLoader = 
			std::make_shared<FakeAudioProcessingLoader>();
		fakeLoader->setLoadCompleteThreshold(2);
		audioLoaderFactory.setLoader(fakeLoader);
		processAudioForSaving();
		assertEqual(2U, fakeLoader->audio().size());
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingLoadsChannelSizeMatchedAudio) {
		std::shared_ptr<FakeAudioProcessingLoader> fakeLoader = 
			std::make_shared<FakeAudioProcessingLoader>();
		audioFrameReader->setChannels(3);
		audioLoaderFactory.setLoader(fakeLoader);
		processAudioForSaving();
		for (auto audio : fakeLoader->audio())
			assertEqual(3U, audio.size());
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingLoadsChunkSizedChannelsWhenUsingHearingAidSimulation
	) {
		std::shared_ptr<FakeAudioProcessingLoader> fakeLoader = 
			std::make_shared<FakeAudioProcessingLoader>();
		audioFrameReader->setChannels(2);
		savingAudio.processing.chunkSize = 4;
		savingAudio.processing.usingHearingAidSimulation = true;
		audioLoaderFactory.setLoader(fakeLoader);
		processAudioForSaving();
		for (auto audio : fakeLoader->audio())
			for (auto channel : audio)
				assertEqual(4U, channel.size());
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingLoadsDefaultFrameSizeChannelsWhenNotUsingHearingAidSimulation
	) {
		std::shared_ptr<FakeAudioProcessingLoader> fakeLoader = 
			std::make_shared<FakeAudioProcessingLoader>();
		audioFrameReader->setChannels(2);
		savingAudio.processing.usingHearingAidSimulation = false;
		audioLoaderFactory.setLoader(fakeLoader);
		processAudioForSaving();
		for (auto audio : fakeLoader->audio())
			for (auto channel : audio) {
				auto expected = gsl::narrow<decltype(channel)::size_type>(
					SpatialHearingAidModel::defaultFramesPerBuffer
				);
				assertEqual(expected, channel.size());
			}
	}

	class RefactoredModelFailureTests : public ::testing::Test {
	protected:
		SpatialHearingAidModel::Testing testing{};
		SpatialHearingAidModel::Trial trial{};
		SpatialHearingAidModel::Calibration calibration{};
		SpatialHearingAidModel::SavingAudio savingAudio{};
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
		AudioFrameWriterStubFactory defaultAudioWriterFactory{};
		AudioFrameWriterFactory *audioWriterFactory{ &defaultAudioWriterFactory };
		AudioPlayerStub defaultPlayer{};
		AudioPlayer *audioPlayer{ &defaultPlayer };
		AudioProcessingLoaderStubFactory defaultAudioLoaderFactory{};
		AudioProcessingLoaderFactory *audioLoaderFactory{ &defaultAudioLoaderFactory };
		SpatializedHearingAidSimulationFactoryStub defaultSimulationFactory{};
		ISpatializedHearingAidSimulationFactory *simulationFactory{&defaultSimulationFactory};
		CalibrationComputerStubFactory defaultCalibrationFactory{};
		ICalibrationComputerFactory *calibrationComputerFactory{ &defaultCalibrationFactory };

		void assertPreparingNewTestThrowsRequestFailure(std::string what) {
			auto model = constructModel();
			try {
				model.prepareNewTest(&testing);
				FAIL() << "Expected SpatialHearingAidModel::RequestFailure.";
			}
			catch (const SpatialHearingAidModel::RequestFailure & e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayTrialThrowsRequestFailure(std::string what) {
			auto model = constructModel();
			try {
				model.playNextTrial(&trial);
				FAIL() << "Expected SpatialHearingAidModel::RequestFailure.";
			}
			catch (const SpatialHearingAidModel::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayCalibrationThrowsRequestFailure(std::string what) {
			auto model = constructModel();
			try {
				model.playCalibration(&calibration);
				FAIL() << "Expected SpatialHearingAidModel::RequestFailure.";
			}
			catch (const SpatialHearingAidModel::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertSaveAudioThrowsRequestFailure(std::string what) {
			auto model = constructModel();
			try {
				model.saveAudio({});
				FAIL() << "Expected SpatialHearingAidModel::RequestFailure.";
			}
			catch (const SpatialHearingAidModel::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
		
		void assertProcessAudioForSavingThrowsRequestFailure(std::string what) {
			auto model = constructModel();
			try {
				model.processAudioForSaving(&savingAudio);
				FAIL() << "Expected SpatialHearingAidModel::RequestFailure.";
			}
			catch (const SpatialHearingAidModel::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
		
		void prepareNewTestIgnoringFailure() {
			auto model = constructModel();
			try {
				model.prepareNewTest(&testing);
			}
			catch (const SpatialHearingAidModel::RequestFailure &) {
			}
		}

		void playTrialIgnoringFailure() {
			auto model = constructModel();
			try {
				model.playNextTrial(&trial);
			}
			catch (const SpatialHearingAidModel::RequestFailure &) {
			}
		}

		SpatialHearingAidModel constructModel() {
			return
			{
				stimulusList,
				documenter,
				audioPlayer,
				audioLoaderFactory,
				audioReaderFactory,
				audioWriterFactory,
				prescriptionReader,
				brirReader,
				simulationFactory,
				calibrationComputerFactory
			};
		}

		void setValidSizesForTest() {
			testing.processing.chunkSize = 1;
			testing.processing.windowSize = 1;
		}

		void setValidSizesForCalibration() {
			calibration.processing.chunkSize = 1;
			calibration.processing.windowSize = 1;
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
		testing.processing.usingHearingAidSimulation = true;
		setValidSizesForTest();
		testing.processing.leftDslPrescriptionFilePath = "a";
		testing.processing.rightDslPrescriptionFilePath = "a";
		assertPreparingNewTestThrowsRequestFailure("Prescription 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		calibration.processing.usingHearingAidSimulation = true;
		setValidSizesForCalibration();
		calibration.processing.leftDslPrescriptionFilePath = "a";
		calibration.processing.rightDslPrescriptionFilePath = "a";
		assertPlayCalibrationThrowsRequestFailure("Prescription 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotDocumentWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		testing.processing.usingHearingAidSimulation = true;
		prepareNewTestIgnoringFailure();
		assertTrue(defaultDocumenter.log().isEmpty());
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		testing.processing.usingSpatialization = true;
		testing.processing.brirFilePath = "a";
		assertPreparingNewTestThrowsRequestFailure("BRIR 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		calibration.processing.usingSpatialization = true;
		calibration.processing.brirFilePath = "a";
		assertPlayCalibrationThrowsRequestFailure("BRIR 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotDocumentWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		testing.processing.usingSpatialization = true;
		prepareNewTestIgnoringFailure();
		assertTrue(defaultDocumenter.log().isEmpty());
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenCoefficientsAreEmpty
	) {
		testing.processing.usingSpatialization = true;
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
		testing.processing.usingHearingAidSimulation = true;
		testing.processing.chunkSize = 0;
		testing.processing.windowSize = 1;
		assertPreparingNewTestThrowsRequestFailure(
			"Both the chunk size and window size must be powers of two; 0 is not a power of two."
		);
		testing.processing.chunkSize = 2;
		testing.processing.windowSize = 3;
		assertPreparingNewTestThrowsRequestFailure(
			"Both the chunk size and window size must be powers of two; 3 is not a power of two."
		);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenChunkSizeIsNotPowerOfTwo
	) {
		calibration.processing.usingHearingAidSimulation = true;
		calibration.processing.chunkSize = 0;
		calibration.processing.windowSize = 1;
		assertPlayCalibrationThrowsRequestFailure(
			"Both the chunk size and window size must be powers of two; 0 is not a power of two."
		);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenWindowSizeIsNotPowerOfTwo
	) {
		calibration.processing.usingHearingAidSimulation = true;
		calibration.processing.chunkSize = 2;
		calibration.processing.windowSize = 3;
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
		processAudioForSavingThrowsRequestFailureWhenAudioFrameReaderCannotBeCreated
	) {
		ErrorAudioFrameReaderFactory failing{ "error." };
		audioReaderFactory = &failing;
		assertProcessAudioForSavingThrowsRequestFailure("error.");
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

	TEST_F(
		RefactoredModelFailureTests,
		saveAudioThrowsRequestFailureWhenAudioFrameWriterCannotBeCreated
	) {
		ErrorAudioFrameWriterFactory failing{ "error." };
		audioWriterFactory = &failing;
		assertSaveAudioThrowsRequestFailure("error.");
	}
}