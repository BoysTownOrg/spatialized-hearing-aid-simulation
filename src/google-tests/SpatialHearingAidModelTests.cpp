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
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <spatialized-hearing-aid-simulation/SpatialHearingAidModel.h>
#include <gtest/gtest.h>

class AudioFrameWriter {

};

class AudioFrameWriterStub : public AudioFrameWriter {

};

class AudioFrameWriterStubFactory {
	std::string filePath_{};
	std::shared_ptr<AudioFrameWriter> writer;
public:
	explicit AudioFrameWriterStubFactory(
		std::shared_ptr<AudioFrameWriter> writer
	) noexcept :
		writer{ std::move(writer) } {}

	std::shared_ptr<AudioFrameWriter> make(std::string filePath) {
		filePath_ = std::move(filePath);
		return writer;
	}

	auto filePath() const {
		return filePath_;
	}
};

namespace {
	class SpatialHearingAidModelTests : public ::testing::Test {
	protected:
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
		AudioLoaderStub audioLoader{};
		SpatializedHearingAidSimulationFactoryStub simulationFactory{};
		std::shared_ptr<CalibrationComputerStub> calibrationComputer =
			std::make_shared<CalibrationComputerStub>();
		CalibrationComputerStubFactory calibrationComputerFactory{ calibrationComputer };
		SpatialHearingAidModel model{
			&stimulusList,
			&documenter,
			&audioPlayer,
			&audioLoader,
			&audioFrameReaderFactory,
			&prescriptionReader,
			&brirReader,
			&simulationFactory,
			&calibrationComputerFactory
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
			SpatialHearingAidModel::SignalProcessing &processing, 
			std::function<void(void)> f
		) {
			processing.chunkSize = 1;
			f();
			assertEqual(1, audioPlayer.preparation().framesPerBuffer);
		}

		void assertAudioPlayerFramesPerBufferMatchesDefault() {
			assertEqual(
				SpatialHearingAidModel::defaultFramesPerBuffer, 
				audioPlayer.preparation().framesPerBuffer
			);
		}

		void assertCalibrationFactoryReceivesAudioFrameReader() noexcept {
			EXPECT_EQ(audioFrameReader.get(), calibrationComputerFactory.reader());
		}

		void assertCalibrationDigitalLevelsAfterCall(double &level, std::function<void(void)> f) {
			audioFrameReader->setChannels(2);
			level = 65;
			f();
			assertEqual(65 - SpatialHearingAidModel::fullScaleLevel_dB_Spl, calibrationComputer->levels().at(0));
			assertEqual(65 - SpatialHearingAidModel::fullScaleLevel_dB_Spl, calibrationComputer->levels().at(1));
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
			SpatialHearingAidModel::SignalProcessing &processing,
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
			SpatialHearingAidModel::SignalProcessing &processing,
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
			SpatialHearingAidModel::SignalProcessing &processing,
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
			SpatialHearingAidModel::SignalProcessing &processing,
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
			SpatialHearingAidModel::SignalProcessing &processing,
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
			SpatialHearingAidModel::SignalProcessing &processing,
			std::function<void(void)> f
		) {
			setFullSimulation(processing);
			assertHearingAidSimulationSampleRateMatchesAudioReaderAfterCall(
				simulationFactory.fullSimulationHearingAid(),
				f
			);
		}

		void assertHearingAidSimulationOnlyYieldsFullScaleLevelMatchingAfterCall(
			SpatialHearingAidModel::SignalProcessing &processing,
			std::function<void(void)> f
		) {
			setHearingAidSimulationOnly(processing);
			assertHearingAidSimulationFullScaleLevelMatchesAfterCall(
				simulationFactory.hearingAidSimulation(),
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
				SpatialHearingAidModel::fullScaleLevel_dB_Spl, 
				hearingAid.at(0).fullScaleLevel_dB_Spl
			);
			assertEqual(
				SpatialHearingAidModel::fullScaleLevel_dB_Spl, 
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
			SpatialHearingAidModel::SignalProcessing &processing,
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
			SpatialHearingAidModel::SignalProcessing &processing,
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
			SpatialHearingAidModel::SignalProcessing &p,
			std::function<void(void)> f
		) {
			p.usingHearingAidSimulation = false;
			f();
			assertFullSimulationNotMade();
			assertHearingAidSimulationOnlyNotMade();
		}

		void assertNoSpatializationYieldsNoSuchSimulationMadeAfterCall(
			SpatialHearingAidModel::SignalProcessing &p,
			std::function<void(void)> f
		) {
			p.usingSpatialization = false;
			f();
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

	TEST_F(SpatialHearingAidModelTests, constructorAssignsAudioLoaderToPlayer) {
		EXPECT_EQ(&audioLoader, audioPlayer.audioLoader());
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		testing.processing.usingHearingAidSimulation = true;
		assertPrescriptionReaderContainsFilePathsAfterCall(
			testing.processing,
			[=]() { prepareNewTest(); }
		);

	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		calibration.processing.usingHearingAidSimulation = true;
		assertPrescriptionReaderContainsFilePathsAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		savingAudio.processing.usingHearingAidSimulation = true;
		assertPrescriptionReaderContainsFilePathsAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		testing.processing.usingHearingAidSimulation = false;
		prepareNewTest();
		assertPrescriptionReaderDidNotReadAnything();
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		calibration.processing.usingHearingAidSimulation = false;
		playCalibration();
		assertPrescriptionReaderDidNotReadAnything();
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		savingAudio.processing.usingHearingAidSimulation = false;
		processAudioForSaving();
		assertPrescriptionReaderDidNotReadAnything();
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		testing.processing.usingSpatialization = true;
		assertBrirReaderReceivesFilePathAfterCall(
			testing.processing,
			[=]() { prepareNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		calibration.processing.usingSpatialization = true;
		assertBrirReaderReceivesFilePathAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		savingAudio.processing.usingSpatialization = true;
		assertBrirReaderReceivesFilePathAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
		testing.processing.usingSpatialization = false;
		prepareNewTest();
		assertBrirReaderDidNotReadAnything();
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationDoesNotReadBrirWhenNotUsingSpatialization) {
		calibration.processing.usingSpatialization = false;
		playCalibration();
		assertBrirReaderDidNotReadAnything();
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingDoesNotReadBrirWhenNotUsingSpatialization) {
		savingAudio.processing.usingSpatialization = false;
		processAudioForSaving();
		assertBrirReaderDidNotReadAnything();
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
		assertAudioReaderFactoryReceivesFilePathAfterCall(
			calibration.audioFilePath,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesAudioFileToFactory) {
		assertAudioReaderFactoryReceivesFilePathAfterCall(
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
		assertAudioPlayerParametersMatchAudioFrameReaderAfterCall([=]() { playNextTrial(); });
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReaderAfterCall([=]() { playCalibration(); });
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
		testing.processing.usingHearingAidSimulation = true;
		assertAudioPlayerFramesPerBufferMatchesProcessingChunkSizeAfterCall(
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation
	) {
		calibration.processing.usingHearingAidSimulation = true;
		assertAudioPlayerFramesPerBufferMatchesProcessingChunkSizeAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playTrialUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		testing.processing.usingHearingAidSimulation = false;
		playFirstTrialOfNewTest();
		assertAudioPlayerFramesPerBufferMatchesDefault();
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		calibration.processing.usingHearingAidSimulation = false;
		playCalibration();
		assertAudioPlayerFramesPerBufferMatchesDefault();
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesAudioFrameReaderToCalibrationFactory) {
		playFirstTrialOfNewTest();
		assertCalibrationFactoryReceivesAudioFrameReader();
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioFrameReaderToCalibrationFactory) {
		playCalibration();
		assertCalibrationFactoryReceivesAudioFrameReader();
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesAudioFrameReaderToCalibrationFactory) {
		processAudioForSaving();
		assertCalibrationFactoryReceivesAudioFrameReader();
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevelsAfterCall(
			trial.level_dB_Spl,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevelsAfterCall(
			calibration.level_dB_Spl,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevelsAfterCall(
			savingAudio.level_dB_Spl,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForTest();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.fullSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForCalibration();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.fullSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForFullSimulation) {
		setFullSimulationForSaving();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.fullSimulationScale(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForTest();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForCalibration();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForHearingAidSimulation) {
		setHearingAidSimulationOnlyForSaving();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.hearingAidSimulationScale(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForTest();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.spatializationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForCalibration();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.spatializationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForSpatialization) {
		setSpatializationOnlyForSaving();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.spatializationScale(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForTest();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.withoutSimulationScale(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForCalibration();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.withoutSimulationScale(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForNoSimulation) {
		setNoSimulationForSaving();
		assertScalarsMatchCalibrationAfterCall(
			simulationFactory.withoutSimulationScale(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForTest();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			testing.processing, 
			simulationFactory.hearingAidSimulation(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForCalibration();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			calibration.processing, 
			simulationFactory.hearingAidSimulation(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		setHearingAidSimulationOnlyForSaving();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			savingAudio.processing, 
			simulationFactory.hearingAidSimulation(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			testing.processing, 
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			calibration.processing, 
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesPrescriptionsToFactoryForFullSimulation
	) {
		setFullSimulationForSaving();
		assertSimulationPrescriptionsMatchPrescriptionReaderAfterCall(
			savingAudio.processing, 
			simulationFactory.fullSimulationHearingAid(),
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMadeAfterCall(
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMadeAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMadeAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMadeAfterCall(
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMadeAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMadeAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsCompressionParametersMatchingAfterCall(
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsCompressionParametersMatchingAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsCompressionParametersMatchingAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsCompressionParametersMatchingAfterCall(
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsCompressionParametersMatchingAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsCompressionParametersMatchingAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertFullSimulationYieldsHearingAidSampleRateMatchingAudioReaderAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsFullScaleLevelMatchingAfterCall(
			testing.processing,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsFullScaleLevelMatchingAfterCall(
			calibration.processing,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertHearingAidSimulationOnlyYieldsFullScaleLevelMatchingAfterCall(
			savingAudio.processing,
			[=]() { processAudioForSaving(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesFullScaleLevelToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertHearingAidSimulationFullScaleLevelMatchesAfterCall(
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesFullScaleLevelToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertHearingAidSimulationFullScaleLevelMatchesAfterCall(
			simulationFactory.fullSimulationHearingAid(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsFullSimulationProcessorsToAudioLoader) {
		setFullSimulationForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.fullSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsFullSimulationProcessorsToAudioLoader) {
		setFullSimulationForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.fullSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsHearingAidSimulationProcessorsToAudioLoader) {
		setHearingAidSimulationOnlyForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.hearingAidSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsHearingAidSimulationProcessorsToAudioLoader) {
		setHearingAidSimulationOnlyForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.hearingAidSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsSpatializationProcessorsToAudioLoader) {
		setSpatializationOnlyForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.spatializationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsSpatializationProcessorsToAudioLoader) {
		setSpatializationOnlyForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.spatializationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsWithoutSimulationProcessorsToAudioLoader) {
		setNoSimulationForTest();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.withoutSimulationProcessors,
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsWithoutSimulationProcessorsToAudioLoader) {
		setNoSimulationForCalibration();
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysAfterCall(
			simulationFactory.withoutSimulationProcessors,
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesBrirToFactoryForSpatialization
	) {
		setSpatializationOnlyForTest();
		assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			simulationFactory.spatialization(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesBrirToFactoryForSpatialization
	) {
		setSpatializationOnlyForCalibration();
		assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			simulationFactory.spatialization(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesBrirToFactoryForFullSimulation
	) {
		setFullSimulationForTest();
		assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			simulationFactory.fullSimulationSpatialization(),
			[=]() { playFirstTrialOfNewTest(); }
		);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesBrirToFactoryForFullSimulation
	) {
		setFullSimulationForCalibration();
		assertSpatializationFilterCoefficientsMatchBrirAfterCall(
			simulationFactory.fullSimulationSpatialization(),
			[=]() { playCalibration(); }
		);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialResetsAudioLoaderBeforePlaying) {
		callWhenPlayerPlays([=]() { assertTrue(audioLoader.log().contains("reset")); });
		playNextTrial();
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationResetsAudioLoaderBeforePlaying) {
		callWhenPlayerPlays([=]() { assertTrue(audioLoader.log().contains("reset")); });
		playCalibration();
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

	class RefactoredModelFailureTests : public ::testing::Test {
	protected:
		SpatialHearingAidModel::Testing testing{};
		SpatialHearingAidModel::Trial trial{};
		SpatialHearingAidModel::Calibration calibration{};
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
				audioLoader,
				audioReaderFactory,
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