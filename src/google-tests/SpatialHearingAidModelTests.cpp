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
	class UseCase {
	public:
		INTERFACE_OPERATIONS(UseCase);
		virtual void run(Model *) = 0;
	};

	class AudioFileUseCase : virtual public UseCase {
	public:
		virtual void setAudioFilePath(std::string) = 0;
	};

	class AudioDeviceUseCase : virtual public UseCase {
	public:
		virtual void setAudioDevice(std::string) = 0;
	};

	class LevelUseCase : virtual public UseCase {
	public:
		virtual void setLevel_dB_Spl(double) = 0;
	};

	class SignalProcessingUseCase : virtual public UseCase {
	public:
		virtual void setHearingAidSimulationOn() = 0;
		virtual void setHearingAidSimulationOff() = 0;
		virtual void setSpatializationOn() = 0;
		virtual void setSpatializationOff() = 0;
		virtual void setAttack_ms(double) = 0;
		virtual void setRelease_ms(double) = 0;
		virtual void setWindowSize(int) = 0;
		virtual void setChunkSize(int) = 0;
		virtual void setLeftDslPrescriptionFilePath(std::string) = 0;
		virtual void setRightDslPrescriptionFilePath(std::string) = 0;
		virtual void setBrirFilePath(std::string) = 0;
	};

	class SignalProcessingWithLevelUseCase :
		virtual public SignalProcessingUseCase,
		virtual public LevelUseCase {};

	class SignalProcessingAudioFileWithLevelUseCase : 
		virtual public SignalProcessingWithLevelUseCase, 
		virtual public AudioFileUseCase {};

	void setHearingAidSimulationOn(Model::SignalProcessing &p) noexcept {
		p.usingHearingAidSimulation = true;
	}

	void setHearingAidSimulationOff(Model::SignalProcessing &p) noexcept {
		p.usingHearingAidSimulation = false;
	}

	void setSpatializationOn(Model::SignalProcessing &p) noexcept {
		p.usingSpatialization = true;
	}

	void setSpatializationOff(Model::SignalProcessing &p) noexcept {
		p.usingSpatialization = false;
	}

	void setAttack_ms(Model::SignalProcessing &p, double x) noexcept {
		p.attack_ms = x;
	}

	void setRelease_ms(Model::SignalProcessing &p, double x) noexcept {
		p.release_ms = x;
	}

	void setChunkSize(Model::SignalProcessing &p, int x) noexcept {
		p.chunkSize = x;
	}

	void setWindowSize(Model::SignalProcessing &p, int x) noexcept {
		p.windowSize = x;
	}

	void setLeftDslPrescriptionFilePath(Model::SignalProcessing &p, std::string s) {
		p.leftDslPrescriptionFilePath = std::move(s);
	}

	void setRightDslPrescriptionFilePath(Model::SignalProcessing &p, std::string s) {
		p.rightDslPrescriptionFilePath = std::move(s);
	}

	void setBrirFilePath(Model::SignalProcessing &p, std::string s) {
		p.brirFilePath = std::move(s);
	}

	class PreparingNewTest : public SignalProcessingUseCase {
		SpatialHearingAidModel::Testing testing{};
	public:
		void run(Model *model) override {
			model->prepareNewTest(testing);
		}

		void setHearingAidSimulationOn() override {
			::setHearingAidSimulationOn(testing.processing);
		}

		void setHearingAidSimulationOff() override {
			::setHearingAidSimulationOff(testing.processing);
		}

		void setSpatializationOn() override {
			::setSpatializationOn(testing.processing);
		}

		void setSpatializationOff() override {
			::setSpatializationOff(testing.processing);
		}
		
		void setAttack_ms(double x) override {
			::setAttack_ms(testing.processing, x);
		}
		
		void setRelease_ms(double x) override {
			::setRelease_ms(testing.processing, x);
		}
		
		void setWindowSize(int x) override {
			::setWindowSize(testing.processing, x);
		}
		
		void setChunkSize(int x) override {
			::setChunkSize(testing.processing, x);
		}

		void setLeftDslPrescriptionFilePath(std::string s) override {
			::setLeftDslPrescriptionFilePath(testing.processing, std::move(s));
		}

		void setRightDslPrescriptionFilePath(std::string s) override {
			::setRightDslPrescriptionFilePath(testing.processing, std::move(s));
		}

		void setBrirFilePath(std::string s) override {
			::setBrirFilePath(testing.processing, std::move(s));
		}

		void setTestFilePath(std::string s) {
			testing.testFilePath = std::move(s);
		}

		void setAudioDirectory(std::string s) {
			testing.audioDirectory = std::move(s);
		}
	};

	class PlayingAudioUseCase : 
		virtual public AudioDeviceUseCase, 
		virtual public LevelUseCase {};

	class PlayingTrial : public PlayingAudioUseCase {
		SpatialHearingAidModel::Trial trial{};
	public:
		void run(Model *model) override {
			model->playNextTrial(trial);
		}

		void setLevel_dB_Spl(double x) override {
			trial.level_dB_Spl = x;
		}

		void setAudioDevice(std::string s) override {
			trial.audioDevice = std::move(s);
		}
	};

	class PlayingFirstTrialOfNewTest : public SignalProcessingWithLevelUseCase {
		PreparingNewTest preparingNewTest{};
		PlayingTrial playingTrial{};
	public:
		void run(Model *model) override {
			preparingNewTest.run(model);
			playingTrial.run(model);
		}

		void setHearingAidSimulationOn() override {
			preparingNewTest.setHearingAidSimulationOn();
		}

		void setHearingAidSimulationOff() override {
			preparingNewTest.setHearingAidSimulationOff();
		}

		void setSpatializationOn() override {
			preparingNewTest.setSpatializationOn();
		}

		void setSpatializationOff() override {
			preparingNewTest.setSpatializationOff();
		}
		
		void setAttack_ms(double x) override {
			preparingNewTest.setAttack_ms(x);
		}
		
		void setRelease_ms(double x) override {
			preparingNewTest.setRelease_ms(x);
		}
		
		void setWindowSize(int x) override {
			preparingNewTest.setWindowSize(x);
		}
		
		void setChunkSize(int x) override {
			preparingNewTest.setChunkSize(x);
		}

		void setLevel_dB_Spl(double x) override {
			playingTrial.setLevel_dB_Spl(x);
		}

		void setLeftDslPrescriptionFilePath(std::string s) override {
			preparingNewTest.setLeftDslPrescriptionFilePath(std::move(s));
		}

		void setRightDslPrescriptionFilePath(std::string s) override {
			preparingNewTest.setRightDslPrescriptionFilePath(std::move(s));
		}

		void setBrirFilePath(std::string s) override {
			preparingNewTest.setBrirFilePath(std::move(s));
		}
	};

	class PlayingCalibration : public SignalProcessingUseCase, public AudioFileUseCase, public PlayingAudioUseCase {
		SpatialHearingAidModel::Calibration calibration{};
	public:
		void run(Model *model) override {
			model->playCalibration(calibration);
		}

		void setHearingAidSimulationOn() override {
			::setHearingAidSimulationOn(calibration.processing);
		}

		void setHearingAidSimulationOff() override {
			::setHearingAidSimulationOff(calibration.processing);
		}

		void setSpatializationOn() override {
			::setSpatializationOn(calibration.processing);
		}

		void setSpatializationOff() override {
			::setSpatializationOff(calibration.processing);
		}
		
		void setAttack_ms(double x) override {
			::setAttack_ms(calibration.processing, x);
		}
		
		void setRelease_ms(double x) override {
			::setRelease_ms(calibration.processing, x);
		}
		
		void setWindowSize(int x) override {
			::setWindowSize(calibration.processing, x);
		}
		
		void setChunkSize(int x) override {
			::setChunkSize(calibration.processing, x);
		}

		void setLevel_dB_Spl(double x) override {
			calibration.level_dB_Spl = x;
		}
		
		void setLeftDslPrescriptionFilePath(std::string s) override {
			::setLeftDslPrescriptionFilePath(calibration.processing, std::move(s));
		}

		void setRightDslPrescriptionFilePath(std::string s) override {
			::setRightDslPrescriptionFilePath(calibration.processing, std::move(s));
		}

		void setBrirFilePath(std::string s) override {
			::setBrirFilePath(calibration.processing, std::move(s));
		}

		void setAudioFilePath(std::string s) override {
			calibration.audioFilePath = std::move(s);
		}

		void setAudioDevice(std::string s) override {
			calibration.audioDevice = std::move(s);
		}
	};

	class ProcessingAudioForSaving : public SignalProcessingAudioFileWithLevelUseCase {
		SpatialHearingAidModel::SavingAudio savingAudio{};
	public:
		void run(Model *model) override {
			model->processAudioForSaving(savingAudio);
		}

		void setHearingAidSimulationOn() override {
			::setHearingAidSimulationOn(savingAudio.processing);
		}

		void setHearingAidSimulationOff() override {
			::setHearingAidSimulationOff(savingAudio.processing);
		}

		void setSpatializationOn() override {
			::setSpatializationOn(savingAudio.processing);
		}

		void setSpatializationOff() override {
			::setSpatializationOff(savingAudio.processing);
		}
		
		void setAttack_ms(double x) override {
			::setAttack_ms(savingAudio.processing, x);
		}
		
		void setRelease_ms(double x) override {
			::setRelease_ms(savingAudio.processing, x);
		}
		
		void setWindowSize(int x) override {
			::setWindowSize(savingAudio.processing, x);
		}
		
		void setChunkSize(int x) override {
			::setChunkSize(savingAudio.processing, x);
		}

		void setLevel_dB_Spl(double x) override {
			savingAudio.level_dB_Spl = x;
		}
		
		void setLeftDslPrescriptionFilePath(std::string s) override {
			::setLeftDslPrescriptionFilePath(savingAudio.processing, std::move(s));
		}

		void setRightDslPrescriptionFilePath(std::string s) override {
			::setRightDslPrescriptionFilePath(savingAudio.processing, std::move(s));
		}

		void setBrirFilePath(std::string s) override {
			::setBrirFilePath(savingAudio.processing, std::move(s));
		}

		void setAudioFilePath(std::string s) override {
			savingAudio.inputAudioFilePath = std::move(s);
		}
	};

	class SavingAudio : public UseCase {
		std::string audioFilePath_{};
	public:
		void run(Model *m) override {
			m->saveAudio(audioFilePath_);
		}

		void setAudioFilePath(std::string s) {
			audioFilePath_ = std::move(s);
		}
	};

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
		
		PreparingNewTest preparingNewTest{};
		PlayingTrial playingTrial{};
		PlayingFirstTrialOfNewTest playingFirstTrialOfNewTest{};
		PlayingCalibration playingCalibration{};
		ProcessingAudioForSaving processingAudioForSaving{};

		SpatialHearingAidModelTests() noexcept {
			setValidDefaults();
		}

		void setValidDefaults() noexcept {
			audioLoader->setComplete();
		}

		void playFirstTrialOfNewTest() {
			prepareNewTest();
			playNextTrial();
		}

		void prepareNewTest() {
			model.prepareNewTest(testing);
		}

		void playNextTrial() {
			model.playNextTrial(trial);
		}

		void playCalibration() {
			model.playCalibration(calibration);
		}

		void processAudioForSaving() {
			model.processAudioForSaving(savingAudio);
		}

		void runUseCase(UseCase *useCase) {
			useCase->run(&model);
		}

		void setSpatializationOnly(SignalProcessingUseCase *useCase) {
			useCase->setSpatializationOn();
			useCase->setHearingAidSimulationOff();
		}

		void setNoSimulation(SignalProcessingUseCase *useCase) {
			useCase->setHearingAidSimulationOff();
			useCase->setSpatializationOff();
		}

		void setHearingAidSimulationOnly(SignalProcessingUseCase *useCase) {
			useCase->setHearingAidSimulationOn();
			useCase->setSpatializationOff();
		}

		void setFullSimulation(SignalProcessingUseCase *useCase) {
			useCase->setHearingAidSimulationOn();
			useCase->setSpatializationOn();
		}
		
		void setInMemoryReader(AudioFileReader &reader_) {
			audioFrameReaderFactory.setReader(std::make_shared<AudioFileInMemory>(reader_));
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

		void assertAudioPlayerHasBeenPlayed(PlayingAudioUseCase *useCase) {
			runUseCase(useCase);
			assertTrue(audioPlayer.played());
		}

		void assertAudioPlayerParametersMatchAudioFrameReader(PlayingAudioUseCase *useCase) {
			audioFrameReader->setChannels(1);
			audioFrameReader->setSampleRate(2);
			runUseCase(useCase);
			assertEqual(1, audioPlayer.preparation().channels);
			assertEqual(2, audioPlayer.preparation().sampleRate);
		}

		void assertAudioPlayerReceivesAudioDevice(AudioDeviceUseCase *useCase) {
			useCase->setAudioDevice("a");
			runUseCase(useCase);
			assertEqual("a", audioPlayer.preparation().audioDevice);
		}

		void assertFramesPerBufferMatchesChunkSizeWhenUsingHearingAidSimulation(SignalProcessingUseCase *useCase) {
			useCase->setChunkSize(1);
			useCase->setHearingAidSimulationOn();
			runUseCase(useCase);
			assertEqual(1, audioPlayer.preparation().framesPerBuffer);
		}

		void assertFramesPerBufferMatchesDefaultWhenNotUsingHearingAidSimulation(SignalProcessingUseCase *useCase) {
			useCase->setHearingAidSimulationOff();
			runUseCase(useCase);
			assertEqual(
				SpatialHearingAidModel::defaultFramesPerBuffer, 
				audioPlayer.preparation().framesPerBuffer
			);
		}

		void assertCalibrationComputerFactoryReceivesAudioFrameReader(SignalProcessingUseCase *useCase) noexcept {
			runUseCase(useCase);
			EXPECT_EQ(audioFrameReader.get(), calibrationComputerFactory.reader());
		}

		void assertCalibrationDigitalLevels(LevelUseCase *useCase) {
			audioFrameReader->setChannels(2);
			useCase->setLevel_dB_Spl(65);
			runUseCase(useCase);
			assertEqual(65 - SpatialHearingAidModel::fullScaleLevel_dB_Spl, calibrationComputer->levels().at(0));
			assertEqual(65 - SpatialHearingAidModel::fullScaleLevel_dB_Spl, calibrationComputer->levels().at(1));
		}

		void assertScalarsMatchCalibrationWhenUsingOnlyHearingAidSimulation(SignalProcessingUseCase *useCase) {
			setHearingAidSimulationOnly(useCase);
			assertScalarsMatchCalibration(
				useCase,
				simulationFactory.hearingAidSimulationScale()
			);
		}

		void assertScalarsMatchCalibration(
			SignalProcessingUseCase *useCase,
			const ArgumentCollection<float> &scalars
		) {
			audioFrameReader->setChannels(2);
			calibrationComputer->addSignalScale(0, 3.3);
			calibrationComputer->addSignalScale(1, 4.4);
			runUseCase(useCase);
			assertEqual(3.3f, scalars.at(0));
			assertEqual(4.4f, scalars.at(1));
		}

		void assertScalarsMatchCalibrationWhenUsingFullSimulation(SignalProcessingUseCase *useCase) {
			setFullSimulation(useCase);
			assertScalarsMatchCalibration(
				useCase,
				simulationFactory.fullSimulationScale()
			);
		}

		void assertScalarsMatchCalibrationWhenUsingOnlySpatialization(SignalProcessingUseCase *useCase) {
			setSpatializationOnly(useCase);
			assertScalarsMatchCalibration(
				useCase,
				simulationFactory.spatializationScale()
			);
		}

		void assertScalarsMatchCalibrationWhenNotUsingSimulation(SignalProcessingUseCase *useCase) {
			setNoSimulation(useCase);
			assertScalarsMatchCalibration(
				useCase,
				simulationFactory.withoutSimulationScale()
			);
		}

		void assertSimulationPrescriptionsMatchReaderWhenUsingOnlyHearingAidSimulation(SignalProcessingUseCase *useCase) {
			setHearingAidSimulationOnly(useCase);
			assertSimulationPrescriptionsMatchPrescriptionReader(
				useCase,
				simulationFactory.hearingAidSimulation()
			);
		}

		void assertSimulationPrescriptionsMatchPrescriptionReader(
			SignalProcessingUseCase *useCase,
			const ArgumentCollection<
				SimulationChannelFactory::HearingAidSimulation> &hearingAid
		) {
			PrescriptionReader::Dsl left;
			left.compressionRatios = { 1 };
			left.crossFrequenciesHz = { 2 };
			left.kneepointGains_dB = { 3 };
			left.kneepoints_dBSpl = { 4 };
			left.broadbandOutputLimitingThresholds_dBSpl = { 5 };
			left.channels = 6;
			useCase->setLeftDslPrescriptionFilePath("leftFilePath");
			prescriptionReader.addPrescription("leftFilePath", left);

			PrescriptionReader::Dsl right;
			right.compressionRatios = { 7 };
			right.crossFrequenciesHz = { 8 };
			right.kneepointGains_dB = { 9 };
			right.kneepoints_dBSpl = { 10 };
			right.broadbandOutputLimitingThresholds_dBSpl = { 11 };
			right.channels = 12;
			useCase->setRightDslPrescriptionFilePath("rightFilePath");
			prescriptionReader.addPrescription("rightFilePath", right);
			
			runUseCase(useCase);

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

		void assertSimulationPrescriptionsMatchReaderWhenUsingFullSimulation(SignalProcessingUseCase *useCase) {
			setFullSimulation(useCase);
			assertSimulationPrescriptionsMatchPrescriptionReader(
				useCase,
				simulationFactory.fullSimulationHearingAid()
			);
		}

		void assertSimulationFactoryReceivesCompressionParametersWhenUsingOnlyHearingAidSimulation(
			SignalProcessingUseCase *useCase
		) {
			setHearingAidSimulationOnly(useCase);
			assertSimulationFactoryReceivesCompressionParameters(
				useCase,
				simulationFactory.hearingAidSimulation()
			);
		}

		void assertSimulationFactoryReceivesCompressionParameters(
			SignalProcessingUseCase *useCase,
			const ArgumentCollection<
				SimulationChannelFactory::HearingAidSimulation> &hearingAid
		) {
			useCase->setAttack_ms(1);
			useCase->setRelease_ms(2);
			useCase->setChunkSize(4);
			useCase->setWindowSize(8);
			
			runUseCase(useCase);

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
			SignalProcessingUseCase *useCase
		) {
			setFullSimulation(useCase);
			assertSimulationFactoryReceivesCompressionParameters(
				useCase,
				simulationFactory.fullSimulationHearingAid()
			);
		}

		void assertSimulationFactoryReceivesSampleRateWhenUsingOnlyHearingAidSimulation(
			SignalProcessingUseCase *useCase
		) {
			setHearingAidSimulationOnly(useCase);
			assertSimulationFactoryReceivesSampleRate(
				useCase,
				simulationFactory.hearingAidSimulation()
			);
		}

		void assertSimulationFactoryReceivesSampleRate(
			SignalProcessingUseCase *useCase,
			const ArgumentCollection<
				SimulationChannelFactory::HearingAidSimulation> &hearingAid
		) {
			audioFrameReader->setSampleRate(1);
			runUseCase(useCase);
			assertEqual(1, hearingAid.at(0).sampleRate);
			assertEqual(1, hearingAid.at(1).sampleRate);
		}

		void assertSimulationFactoryReceivesSampleRateWhenUsingFullSimulation(
			SignalProcessingUseCase *useCase
		) {
			setFullSimulation(useCase);
			assertSimulationFactoryReceivesSampleRate(
				useCase,
				simulationFactory.fullSimulationHearingAid()
			);
		}

		void assertSimulationFactoryReceivesFullScaleLevelWhenUsingOnlyHearingAidSimulation(
			SignalProcessingUseCase *useCase
		) {
			setHearingAidSimulationOnly(useCase);
			assertHearingAidSimulationFullScaleLevelMatches(
				useCase,
				simulationFactory.hearingAidSimulation()
			);
		}

		void assertHearingAidSimulationFullScaleLevelMatches(
			SignalProcessingUseCase *useCase,
			const ArgumentCollection<
				SimulationChannelFactory::HearingAidSimulation> &hearingAid
		) {
			runUseCase(useCase);
			assertEqual(
				SpatialHearingAidModel::fullScaleLevel_dB_Spl, 
				hearingAid.at(0).fullScaleLevel_dB_Spl
			);
			assertEqual(
				SpatialHearingAidModel::fullScaleLevel_dB_Spl, 
				hearingAid.at(1).fullScaleLevel_dB_Spl
			);
		}

		void assertSimulationFactoryReceivesFullScaleLevelWhenUsingFullSimulation(
			SignalProcessingUseCase *useCase
		) {
			setFullSimulation(useCase);
			assertHearingAidSimulationFullScaleLevelMatches(
				useCase,
				simulationFactory.fullSimulationHearingAid()
			);
		}

		void assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingFullSimulation(
			SignalProcessingUseCase *useCase
		) {
			setFullSimulation(useCase);
			assertAudioLoaderAppliesSimulationWhenPlayerPlays(
				useCase,
				simulationFactory.fullSimulationProcessors
			);
		}

		void assertAudioLoaderAppliesSimulationWhenPlayerPlays(
			SignalProcessingUseCase *useCase,
			PoppableVector<std::shared_ptr<SignalProcessor>> &processors
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
			
			runUseCase(useCase);
			
			assertEqual({ 5 * 2 }, left);
			assertEqual({ 7 * 3 }, right);
		}

		void assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingOnlyHearingAidSimulation(
			SignalProcessingUseCase *useCase
		) {
			setHearingAidSimulationOnly(useCase);
			assertAudioLoaderAppliesSimulationWhenPlayerPlays(
				useCase,
				simulationFactory.hearingAidSimulationProcessors
			);
		}

		void assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingOnlySpatialization(
			SignalProcessingUseCase *useCase
		) {
			setSpatializationOnly(useCase);
			assertAudioLoaderAppliesSimulationWhenPlayerPlays(
				useCase,
				simulationFactory.spatializationProcessors
			);
		}

		void assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingNoSimulation(
			SignalProcessingUseCase *useCase
		) {
			setNoSimulation(useCase);
			assertAudioLoaderAppliesSimulationWhenPlayerPlays(
				useCase,
				simulationFactory.withoutSimulationProcessors
			);
		}

		void assertAudioLoaderAppliesSimulationWhenUsingNoSimulation(
			SignalProcessingUseCase *useCase
		) {
			setNoSimulation(useCase);
			assertAudioLoaderAppliesSimulation(
				useCase,
				simulationFactory.withoutSimulationProcessors
			);
		}

		void assertAudioLoaderAppliesSimulation(
			SignalProcessingUseCase *useCase,
			PoppableVector<std::shared_ptr<SignalProcessor>> &processors
		) {
			std::vector<std::shared_ptr<SignalProcessor>> simulation = {
				std::make_shared<MultipliesSamplesBy>(2.0f),
				std::make_shared<MultipliesSamplesBy>(3.0f)
			};
			processors.set(simulation);
			runUseCase(useCase);

			buffer_type left = { 5 };
			buffer_type right = { 7 };
			std::vector<channel_type> channels = { left, right };

			processAudioLoaderProcessor(channels);
			
			assertEqual({ 5 * 2 }, left);
			assertEqual({ 7 * 3 }, right);
		}

		void assertSpatializationFilterCoefficientsMatchBrirWhenUsingOnlySpatialization(SignalProcessingUseCase *useCase) {
			setSpatializationOnly(useCase);
			assertSpatializationFilterCoefficientsMatchBrir(
				useCase,
				simulationFactory.spatialization()
			);
		}

		void assertSpatializationFilterCoefficientsMatchBrir(
			SignalProcessingUseCase *useCase,
			const ArgumentCollection<
				SimulationChannelFactory::Spatialization> &spatialization
		) {
			BrirReader::BinauralRoomImpulseResponse brir;
			brir.left = { 1, 2 };
			brir.right = { 3, 4 };
			brirReader.setBrir(brir);
			runUseCase(useCase);
			assertEqual({ 1, 2, }, spatialization.at(0).filterCoefficients);
			assertEqual({ 3, 4, }, spatialization.at(1).filterCoefficients);
		}

		void assertSpatializationFilterCoefficientsMatchBrirWhenUsingFullSimulation(SignalProcessingUseCase *useCase) {
			setFullSimulation(useCase);
			assertSpatializationFilterCoefficientsMatchBrir(
				useCase,
				simulationFactory.fullSimulationSpatialization()
			);
		}

		void assertPrescriptionReaderReceivesFilePathsWhenUsingHearingAidSimulation(
			SignalProcessingUseCase *useCase
		) {
			useCase->setHearingAidSimulationOn();
			useCase->setLeftDslPrescriptionFilePath("a");
			useCase->setRightDslPrescriptionFilePath("b");
			runUseCase(useCase);
			assertTrue(prescriptionReader.filePaths().contains("a"));
			assertTrue(prescriptionReader.filePaths().contains("b"));
		}

		void assertPrescriptionReaderDoesNotReadWhenNotUsingHearingAidSimulation(
			SignalProcessingUseCase *useCase
		) {
			useCase->setHearingAidSimulationOff();
			runUseCase(useCase);
			assertTrue(prescriptionReader.filePaths().empty());
		}

		void assertBrirReaderReceivesFilePathWhenUsingSpatialization(
			SignalProcessingUseCase *useCase
		) {
			useCase->setSpatializationOn();
			useCase->setBrirFilePath("a");
			runUseCase(useCase);
			assertEqual("a", brirReader.filePath());
		}

		void assertAudioReaderFactoryReceivesFilePath(
			AudioFileUseCase *useCase
		) {
			useCase->setAudioFilePath("a");
			runUseCase(useCase);
			assertEqual("a", audioFrameReaderFactory.filePath());
		}
		
		void assertBrirReaderDoesNotReadWhenNotUsingSpatialization(
			SignalProcessingUseCase *useCase
		) {
			useCase->setSpatializationOff();
			runUseCase(useCase);
			assertFalse(brirReader.readCalled());
		}

		void assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(
			LevelUseCase *useCase
		) {
			callWhenPlayerPlays([=]() { assertAudioFrameReaderPassedToLoaderFactory(); });
			runUseCase(useCase);
		}

		void assertAudioFrameReaderPassedToLoaderFactory() noexcept {
			EXPECT_EQ(audioFrameReader, audioLoaderFactory.audioFrameReader());
		}

		void assertPlayerPreparedPriorToPlaying(UseCase *useCase) {
			runUseCase(useCase);
			assertEqual("prepareToPlay play ", audioPlayer.log());
		}

		void assertAudioLoaderHasNotBeenModifiedWhenPlayerIsPlaying(UseCase *useCase) {
			audioPlayer.setPlaying();
			runUseCase(useCase);
			assertAudioLoaderHasNotBeenModified();
		}

		void assertNoHearingAidSimulationYieldsNoSuchSimulationMade(
			SignalProcessingUseCase *useCase
		) {
			useCase->setHearingAidSimulationOff();
			runUseCase(useCase);
			assertFullSimulationNotMade();
			assertHearingAidSimulationOnlyNotMade();
		}

		void assertNoSpatializationYieldsNoSuchSimulationMade(
			SignalProcessingUseCase *useCase
		) {
			useCase->setSpatializationOff();
			runUseCase(useCase);
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
		assertPrescriptionReaderReceivesFilePathsWhenUsingHearingAidSimulation(&preparingNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		assertPrescriptionReaderReceivesFilePathsWhenUsingHearingAidSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingPassesPrescriptionFilePathsToReaderWhenUsingHearingAidSimulation
	) {
		assertPrescriptionReaderReceivesFilePathsWhenUsingHearingAidSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		assertPrescriptionReaderDoesNotReadWhenNotUsingHearingAidSimulation(&preparingNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		assertPrescriptionReaderDoesNotReadWhenNotUsingHearingAidSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingDoesNotReadPrescriptionsWhenNotUsingHearingAidSimulation
	) {
		assertPrescriptionReaderDoesNotReadWhenNotUsingHearingAidSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		prepareNewTestPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		assertBrirReaderReceivesFilePathWhenUsingSpatialization(&preparingNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		assertBrirReaderReceivesFilePathWhenUsingSpatialization(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		processAudioForSavingPassesBrirFilePathToReaderWhenUsingSpatialization
	) {
		assertBrirReaderReceivesFilePathWhenUsingSpatialization(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, prepareNewTestDoesNotReadBrirWhenNotUsingSpatialization) {
		assertBrirReaderDoesNotReadWhenNotUsingSpatialization(&preparingNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationDoesNotReadBrirWhenNotUsingSpatialization) {
		assertBrirReaderDoesNotReadWhenNotUsingSpatialization(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingDoesNotReadBrirWhenNotUsingSpatialization) {
		assertBrirReaderDoesNotReadWhenNotUsingSpatialization(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPlaysPlayer) {
		assertAudioPlayerHasBeenPlayed(&playingTrial);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPlaysPlayer) {
		assertAudioPlayerHasBeenPlayed(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioFileToFactory) {
		assertAudioReaderFactoryReceivesFilePath(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesAudioFileToFactory) {
		assertAudioReaderFactoryReceivesFilePath(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(&playingTrial);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioFrameReaderToAudioLoaderPriorToPlaying) {
		assertAudioFrameReaderPassedToLoaderWhenPlayerPlaysDuringCall(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReader(&playingTrial);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesReaderMatchedParametersToPlayer) {
		assertAudioPlayerParametersMatchAudioFrameReader(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesAudioDeviceToPlayer) {
		assertAudioPlayerReceivesAudioDevice(&playingTrial);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioDeviceToPlayer) {
		assertAudioPlayerReceivesAudioDevice(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation
	) {
		assertFramesPerBufferMatchesChunkSizeWhenUsingHearingAidSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationUsesChunkSizeAsFramesPerBufferWhenUsingHearingAidSimulation
	) {
		assertFramesPerBufferMatchesChunkSizeWhenUsingHearingAidSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playTrialUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		assertFramesPerBufferMatchesDefaultWhenNotUsingHearingAidSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests,
		playCalibrationUsesDefaultFramesPerBufferWhenNotUsingHearingAidSimulation
	) {
		assertFramesPerBufferMatchesDefaultWhenNotUsingHearingAidSimulation(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesAudioFrameReaderToCalibrationFactory) {
		assertCalibrationComputerFactoryReceivesAudioFrameReader(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesAudioFrameReaderToCalibrationFactory) {
		assertCalibrationComputerFactoryReceivesAudioFrameReader(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesAudioFrameReaderToCalibrationFactory) {
		assertCalibrationComputerFactoryReceivesAudioFrameReader(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevels(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevels(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesDigitalLevelToCalibrationComputer) {
		assertCalibrationDigitalLevels(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForFullSimulation) {
		assertScalarsMatchCalibrationWhenUsingFullSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForFullSimulation) {
		assertScalarsMatchCalibrationWhenUsingFullSimulation(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForFullSimulation) {
		assertScalarsMatchCalibrationWhenUsingFullSimulation(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForHearingAidSimulation) {
		assertScalarsMatchCalibrationWhenUsingOnlyHearingAidSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForHearingAidSimulation) {
		assertScalarsMatchCalibrationWhenUsingOnlyHearingAidSimulation(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForHearingAidSimulation) {
		assertScalarsMatchCalibrationWhenUsingOnlyHearingAidSimulation(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForSpatialization) {
		assertScalarsMatchCalibrationWhenUsingOnlySpatialization(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForSpatialization) {
		assertScalarsMatchCalibrationWhenUsingOnlySpatialization(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForSpatialization) {
		assertScalarsMatchCalibrationWhenUsingOnlySpatialization(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialComputesCalibrationScalarsForNoSimulation) {
		assertScalarsMatchCalibrationWhenNotUsingSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationComputesCalibrationScalarsForNoSimulation) {
		assertScalarsMatchCalibrationWhenNotUsingSimulation(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingComputesCalibrationScalarsForNoSimulation) {
		assertScalarsMatchCalibrationWhenNotUsingSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		assertSimulationPrescriptionsMatchReaderWhenUsingOnlyHearingAidSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		assertSimulationPrescriptionsMatchReaderWhenUsingOnlyHearingAidSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesPrescriptionsToFactoryForHearingAidSimulation
	) {
		assertSimulationPrescriptionsMatchReaderWhenUsingOnlyHearingAidSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesPrescriptionsToFactoryForFullSimulation
	) {
		assertSimulationPrescriptionsMatchReaderWhenUsingFullSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesPrescriptionsToFactoryForFullSimulation
	) {
		assertSimulationPrescriptionsMatchReaderWhenUsingFullSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesPrescriptionsToFactoryForFullSimulation
	) {
		assertSimulationPrescriptionsMatchReaderWhenUsingFullSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMade(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMade(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingDoesNotMakeHearingAidSimulationOrFullSimulationWhenNotUsingHearingAidSimulation
	) {
		assertNoHearingAidSimulationYieldsNoSuchSimulationMade(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMade(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMade(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingDoesNotMakeSpatializationOrFullSimulationWhenNotUsingSpatialization
	) {
		assertNoSpatializationYieldsNoSuchSimulationMade(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingOnlyHearingAidSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingOnlyHearingAidSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesCompressionParametersToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingOnlyHearingAidSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingFullSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingFullSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesCompressionParametersToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesCompressionParametersWhenUsingFullSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingOnlyHearingAidSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingOnlyHearingAidSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesAudioReaderSampleRateToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingOnlyHearingAidSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingFullSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingFullSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesAudioReaderSampleRateToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesSampleRateWhenUsingFullSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesFullScaleLevelWhenUsingOnlyHearingAidSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesFullScaleLevelWhenUsingOnlyHearingAidSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesFullScaleLevelToFactoryForHearingAidSimulation
	) {
		assertSimulationFactoryReceivesFullScaleLevelWhenUsingOnlyHearingAidSimulation(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesFullScaleLevelToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesFullScaleLevelWhenUsingFullSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesFullScaleLevelToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesFullScaleLevelWhenUsingFullSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesFullScaleLevelToFactoryForFullSimulation
	) {
		assertSimulationFactoryReceivesFullScaleLevelWhenUsingFullSimulation(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsFullSimulationProcessorsToAudioLoader) {
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingFullSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsFullSimulationProcessorsToAudioLoader) {
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingFullSimulation(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsHearingAidSimulationProcessorsToAudioLoader) {
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingOnlyHearingAidSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsHearingAidSimulationProcessorsToAudioLoader) {
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingOnlyHearingAidSimulation(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsSpatializationProcessorsToAudioLoader) {
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingOnlySpatialization(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsSpatializationProcessorsToAudioLoader) {
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingOnlySpatialization(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialAssignsWithoutSimulationProcessorsToAudioLoader) {
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingNoSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationAssignsWithoutSimulationProcessorsToAudioLoader) {
		assertAudioLoaderAppliesSimulationWhenPlayerPlaysWhenUsingNoSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesBrirToFactoryForSpatialization
	) {
		assertSpatializationFilterCoefficientsMatchBrirWhenUsingOnlySpatialization(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesBrirToFactoryForSpatialization
	) {
		assertSpatializationFilterCoefficientsMatchBrirWhenUsingOnlySpatialization(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesBrirToFactoryForSpatialization
	) {
		assertSpatializationFilterCoefficientsMatchBrirWhenUsingOnlySpatialization(&processingAudioForSaving);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playTrialPassesBrirToFactoryForFullSimulation
	) {
		assertSpatializationFilterCoefficientsMatchBrirWhenUsingFullSimulation(&playingFirstTrialOfNewTest);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		playCalibrationPassesBrirToFactoryForFullSimulation
	) {
		assertSpatializationFilterCoefficientsMatchBrirWhenUsingFullSimulation(&playingCalibration);
	}

	TEST_F(
		SpatialHearingAidModelTests, 
		processAudioForSavingPassesBrirToFactoryForFullSimulation
	) {
		assertSpatializationFilterCoefficientsMatchBrirWhenUsingFullSimulation(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialPreparesPlayerBeforePlaying) {
		assertPlayerPreparedPriorToPlaying(&playingTrial);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationPreparesPlayerBeforePlaying) {
		assertPlayerPreparedPriorToPlaying(&playingCalibration);
	}

	TEST_F(SpatialHearingAidModelTests, playTrialDoesNotAlterLoaderWhenPlayerPlaying) {
		assertAudioLoaderHasNotBeenModifiedWhenPlayerIsPlaying(&playingTrial);
	}

	TEST_F(SpatialHearingAidModelTests, playCalibrationDoesNotAlterLoaderWhenPlayerPlaying) {
		assertAudioLoaderHasNotBeenModifiedWhenPlayerIsPlaying(&playingCalibration);
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

	TEST_F(SpatialHearingAidModelTests, processAudioForSavingPassesProcessorToAudioLoaderFactory) {
		assertAudioLoaderAppliesSimulationWhenUsingNoSimulation(&processingAudioForSaving);
	}

	TEST_F(SpatialHearingAidModelTests, savingAudioWritesPreviouslyProcessedAudio) {
		std::shared_ptr<FakeAudioProcessingLoader2> fakeLoader = 
			std::make_shared<FakeAudioProcessingLoader2>();
		fakeLoader->setAudioToLoad({ 1, 2, 3, 4, 5, 6, 7, 8 });
		audioFrameReader->setChannels(2);
		savingAudio.processing.chunkSize = 4;
		savingAudio.processing.usingHearingAidSimulation = true;
		audioLoaderFactory.setLoader(fakeLoader);
		processAudioForSaving();
		model.saveAudio({});
		assertEqual({ 1, 2, 3, 4, 5, 6, 7, 8 }, audioFrameWriter->written());
	}

	class RefactoredModelFailureTests : public ::testing::Test {
	protected:
		PreparingNewTest preparingNewTest{};
		PlayingTrial playingTrial{};
		PlayingFirstTrialOfNewTest playingFirstTrialOfNewTest{};
		PlayingCalibration playingCalibration{};
		ProcessingAudioForSaving processingAudioForSaving{};
		SavingAudio savingAudio{};
		PrescriptionReaderStub defaultPrescriptionReader{};
		PrescriptionReader *prescriptionReader{ &defaultPrescriptionReader };
		BrirReaderStub defaultBrirReader{};
		BrirReader *brirReader{ &defaultBrirReader };
		FakeStimulusList defaultStimulusList{};
		StimulusList *stimulusList{ &defaultStimulusList };
		DocumenterStub defaultDocumenter{};
		TestDocumenter *documenter{ &defaultDocumenter };
		AudioFrameReaderStubFactory defaultAudioReaderFactory{};
		AudioFrameReaderFactory *audioReaderFactory{ &defaultAudioReaderFactory };
		AudioFrameWriterStubFactory defaultAudioWriterFactory{};
		AudioFrameWriterFactory *audioWriterFactory{ &defaultAudioWriterFactory };
		AudioPlayerStub defaultPlayer{};
		AudioPlayer *audioPlayer{ &defaultPlayer };
		AudioProcessingLoaderStubFactory defaultAudioLoaderFactory{};
		AudioProcessingLoaderFactory *audioLoaderFactory{ &defaultAudioLoaderFactory };
		SpatializedHearingAidSimulationFactoryStub defaultSimulationFactory{};
		SimulationChannelFactory *simulationFactory{&defaultSimulationFactory};
		CalibrationComputerStubFactory defaultCalibrationFactory{};
		CalibrationComputerFactory *calibrationComputerFactory{ &defaultCalibrationFactory };

		void assertThrowsRequestFailure(UseCase *useCase, std::string what) {
			try {
				runUseCase(useCase);
				FAIL() << "Expected SpatialHearingAidModel::RequestFailure.";
			}
			catch (const SpatialHearingAidModel::RequestFailure & e) {
				assertEqual(std::move(what), e.what());
			}
		}
		
		void runUseCase(UseCase *useCase) {
			auto model = constructModel();
			useCase->run(&model);
		}

		void ignoreRequestFailure(UseCase *useCase) {
			try {
				runUseCase(useCase);
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

		void assertThrowsRequestFailureWhenPrescriptionReaderFails(SignalProcessingUseCase *useCase) {
			FailingPrescriptionReader failing;
			prescriptionReader = &failing;
			useCase->setHearingAidSimulationOn();
			useCase->setChunkSize(1);
			useCase->setWindowSize(1);
			useCase->setLeftDslPrescriptionFilePath("a");
			useCase->setRightDslPrescriptionFilePath("a");
			assertThrowsRequestFailure(useCase, "Prescription 'a' cannot be read.");
		}

		void assertThrowsRequestFailureWhenBrirReaderFails(SignalProcessingUseCase *useCase) {
			FailingBrirReader failing;
			brirReader = &failing;
			useCase->setSpatializationOn();
			useCase->setBrirFilePath("a");
			assertThrowsRequestFailure(useCase, "BRIR 'a' cannot be read.");
		}

		void assertThrowsRequestFailureWhenProcessingSizesNotPowersOfTwo(SignalProcessingUseCase *useCase) {
			useCase->setHearingAidSimulationOn();
			useCase->setChunkSize(0);
			useCase->setWindowSize(1);
			assertThrowsRequestFailure(
				useCase,
				"Both the chunk size and window size must be powers of two; 0 is not a power of two."
			);
			useCase->setChunkSize(2);
			useCase->setWindowSize(3);
			assertThrowsRequestFailure(
				useCase,
				"Both the chunk size and window size must be powers of two; 3 is not a power of two."
			);
		}

		void assertThrowsRequestFailureWhenBrirCoefficientsEmpty(SignalProcessingUseCase *useCase) {
			useCase->setSpatializationOn();
			BrirReader::BinauralRoomImpulseResponse brir;
			brir.left = {};
			brir.right = { 0 };
			defaultBrirReader.setBrir(brir);
			assertThrowsRequestFailure(
				useCase,
				"The left BRIR coefficients are empty, therefore a filter operation cannot be defined."
			);
			brir.left = { 0 };
			brir.right = {};
			defaultBrirReader.setBrir(brir);
			assertThrowsRequestFailure(
				useCase,
				"The right BRIR coefficients are empty, therefore a filter operation cannot be defined."
			);
		}

		void assertThrowsRequestFailureWhenAudioReaderFactoryFails(AudioFileUseCase *useCase) {
			ErrorAudioFrameReaderFactory failing{};
			audioReaderFactory = &failing;
			useCase->setAudioFilePath("a");
			assertThrowsRequestFailure(useCase, "Audio file 'a' cannot be read.");
		}

		void assertThrowsRequestFailureWhenAudioPlayerFailsToPrepare(AudioDeviceUseCase *useCase) {
			PreparationFailingAudioPlayer failing;
			audioPlayer = &failing;
			useCase->setAudioDevice("a");
			assertThrowsRequestFailure(useCase, "Audio device 'a' cannot be opened.");
		}

		void assertDocumenterLogIsStillEmpty(SignalProcessingUseCase *useCase) {
			ignoreRequestFailure(useCase);
			assertTrue(defaultDocumenter.log().isEmpty());
		}
	};

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsInitializationFailureWhenDocumenterFailsToInitialize
	) {
		InitializationFailingDocumenter failing;
		documenter = &failing;
		preparingNewTest.setTestFilePath("a");
		assertThrowsRequestFailure(&preparingNewTest, "Test file 'a' cannot be written.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenStimulusListFails
	) {
		FailsToInitializeStimulusList failing;
		stimulusList = &failing;
		preparingNewTest.setAudioDirectory("a");
		assertThrowsRequestFailure(&preparingNewTest, "Stimulus list 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotDocumentWhenPrescriptionReaderFails
	) {
		FailingPrescriptionReader failing;
		prescriptionReader = &failing;
		preparingNewTest.setHearingAidSimulationOn();
		assertDocumenterLogIsStillEmpty(&preparingNewTest);
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestDoesNotDocumentWhenBrirReaderFails
	) {
		FailingBrirReader failing;
		brirReader = &failing;
		preparingNewTest.setSpatializationOn();
		assertDocumenterLogIsStillEmpty(&preparingNewTest);
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenPrescriptionReaderFails
	) {
		assertThrowsRequestFailureWhenPrescriptionReaderFails(&preparingNewTest);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenPrescriptionReaderFails
	) {
		assertThrowsRequestFailureWhenPrescriptionReaderFails(&playingCalibration);
	}

	TEST_F(
		RefactoredModelFailureTests,
		processAudioForSavingThrowsRequestFailureWhenPrescriptionReaderFails
	) {
		assertThrowsRequestFailureWhenPrescriptionReaderFails(&processingAudioForSaving);
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenBrirReaderFails
	) {
		assertThrowsRequestFailureWhenBrirReaderFails(&preparingNewTest);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenBrirReaderFails
	) {
		assertThrowsRequestFailureWhenBrirReaderFails(&playingCalibration);
	}

	TEST_F(
		RefactoredModelFailureTests,
		processAudioForSavingThrowsRequestFailureWhenBrirReaderFails
	) {
		assertThrowsRequestFailureWhenBrirReaderFails(&processingAudioForSaving);
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenCoefficientsAreEmpty
	) {
		assertThrowsRequestFailureWhenBrirCoefficientsEmpty(&preparingNewTest);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenCoefficientsAreEmpty
	) {
		assertThrowsRequestFailureWhenBrirCoefficientsEmpty(&playingCalibration);
	}

	TEST_F(
		RefactoredModelFailureTests,
		processAudioForSavingThrowsRequestFailureWhenCoefficientsAreEmpty
	) {
		assertThrowsRequestFailureWhenBrirCoefficientsEmpty(&processingAudioForSaving);
	}

	TEST_F(
		RefactoredModelFailureTests,
		prepareNewTestThrowsRequestFailureWhenWindowOrChunkSizeIsNotPowerOfTwo
	) {
		assertThrowsRequestFailureWhenProcessingSizesNotPowersOfTwo(&preparingNewTest);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenChunkSizeIsNotPowerOfTwo
	) {
		assertThrowsRequestFailureWhenProcessingSizesNotPowersOfTwo(&playingCalibration);
	}

	TEST_F(
		RefactoredModelFailureTests,
		processAudioForSavingThrowsRequestFailureWhenChunkSizeIsNotPowerOfTwo
	) {
		assertThrowsRequestFailureWhenProcessingSizesNotPowersOfTwo(&processingAudioForSaving);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playTrialThrowsRequestFailureWhenAudioFrameReaderCannotBeCreated
	) {
		ErrorAudioFrameReaderFactory failing{};
		audioReaderFactory = &failing;
		defaultStimulusList.setContents({ "a" });
		assertThrowsRequestFailure(&playingFirstTrialOfNewTest, "Audio file 'a' cannot be read.");
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenAudioFrameReaderCannotBeCreated
	) {
		assertThrowsRequestFailureWhenAudioReaderFactoryFails(&playingCalibration);
	}

	TEST_F(
		RefactoredModelFailureTests,
		processAudioForSavingThrowsRequestFailureWhenAudioFrameReaderCannotBeCreated
	) {
		assertThrowsRequestFailureWhenAudioReaderFactoryFails(&processingAudioForSaving);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playTrialThrowsRequestFailureWhenPlayerThrowsPreparationFailure
	) {
		assertThrowsRequestFailureWhenAudioPlayerFailsToPrepare(&playingTrial);
	}

	TEST_F(
		RefactoredModelFailureTests,
		playCalibrationThrowsRequestFailureWhenPlayerThrowsPreparationFailure
	) {
		assertThrowsRequestFailureWhenAudioPlayerFailsToPrepare(&playingCalibration);
	}

	TEST_F(RefactoredModelFailureTests, playTrialDoesNotAdvanceStimulusWhenPlayerFails) {
		PreparationFailingAudioPlayer failing;
		audioPlayer = &failing;
		defaultStimulusList.setContents({ "a", "b", "c" });
		ignoreRequestFailure(&playingTrial);
		assertEqual("a", defaultStimulusList.next());
	}

	TEST_F(
		RefactoredModelFailureTests,
		saveAudioThrowsRequestFailureWhenAudioFrameWriterCannotBeCreated
	) {
		ErrorAudioFrameWriterFactory failing{};
		audioWriterFactory = &failing;
		savingAudio.setAudioFilePath("a");
		assertThrowsRequestFailure(&savingAudio, "Audio file 'a' cannot be written.");
	}
}