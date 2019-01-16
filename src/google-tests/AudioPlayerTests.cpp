#include "AudioDeviceStub.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFileReader.h"
#include "assert-utility.h"
#include <playing-audio/AudioPlayer.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

namespace {
	class AudioPlayerTests : public ::testing::Test {
	protected:
		AudioDeviceStub device{};
		std::shared_ptr<AudioFrameReaderStub> frameReader = std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory readerFactory{ frameReader };
		std::shared_ptr<AudioFrameProcessorStub> processor = std::make_shared<AudioFrameProcessorStub>();
		AudioFrameProcessorStubFactory processorFactory{ processor };
		AudioPlayer player{ &device, &readerFactory, &processorFactory };

		void assertPlayThrowsDeviceFailureWithMessage(std::string errorMessage) {
			try {
				player.play({});
				FAIL() << "Expected AudioPlayer::RequestFailure";
			}
			catch (const AudioPlayer::RequestFailure &e) {
				assertEqual(std::move(errorMessage), e.what());
			}
		}

		void play(AudioPlayer::PlayRequest r = {}) {
			player.play(std::move(r));
		}

		void fillStreamBuffer(void *x = {}, int n = {}) {
			device.fillStreamBuffer(x, n);
		}
	};

	TEST_F(AudioPlayerTests, constructorSetsItselfAsDeviceController) {
		EXPECT_EQ(&player, device.controller());
	}

	TEST_F(AudioPlayerTests, playFirstClosesStreamThenOpensThenStarts) {
		play();
		assertEqual("close open start ", device.streamLog());
	}

	TEST_F(AudioPlayerTests, playWhileStreamingDoesNotAlterStream) {
		device.setStreaming();
		play();
		EXPECT_TRUE(device.streamLog().empty());
	}

	TEST_F(AudioPlayerTests, fillStreamBufferPadsZeroToEndOfInput) {
		frameReader->setChannels(1);
		play();
		frameReader->setComplete();
		std::vector<float> audio(3, -1);
		float *x[]{ &audio.front() };
		fillStreamBuffer(x, 3);
		EXPECT_EQ(0, processor->audioBuffer()[0][0]);
		EXPECT_EQ(0, processor->audioBuffer()[0][1]);
		EXPECT_EQ(0, processor->audioBuffer()[0][2]);
	}

	TEST_F(AudioPlayerTests, fillStreamBufferSetsCallbackResultToCompleteAfterProcessingPaddedZeroes) {
		frameReader->setChannels(1);
		play();
		frameReader->setComplete();
		processor->setGroupDelay(3);
		float left{};
		float *x[]{ &left };
		fillStreamBuffer(x, 1);
		EXPECT_FALSE(device.complete());
		fillStreamBuffer(x, 1);
		EXPECT_FALSE(device.complete());
		fillStreamBuffer(x, 1);
		EXPECT_TRUE(device.complete());
		play();
		fillStreamBuffer(x, 1);
		EXPECT_FALSE(device.complete());
	}

	TEST_F(AudioPlayerTests, fillStreamBufferPassesAudio) {
		frameReader->setChannels(2);
		play();
		float left{};
		float right{};
		float *x[]{ &left, &right };
		fillStreamBuffer(x, 1);
		EXPECT_EQ(&left, frameReader->audioBuffer()[0].data());
		EXPECT_EQ(&right, frameReader->audioBuffer()[1].data());
		EXPECT_EQ(1, frameReader->audioBuffer()[0].size());
		EXPECT_EQ(1, frameReader->audioBuffer()[1].size());
		EXPECT_EQ(&left, processor->audioBuffer()[0].data());
		EXPECT_EQ(&right, processor->audioBuffer()[1].data());
		EXPECT_EQ(1, processor->audioBuffer()[0].size());
		EXPECT_EQ(1, processor->audioBuffer()[1].size());
	}

	TEST_F(AudioPlayerTests, initializePassesParametersToFactories) {
		StimulusPlayer::Initialization initialization;
		initialization.leftDslPrescriptionFilePath = "a";
		initialization.rightDslPrescriptionFilePath = "b";
		initialization.brirFilePath = "c";
		initialization.max_dB_Spl = 1;
		initialization.attack_ms = 2;
		initialization.release_ms = 3;
		initialization.windowSize = 4;
		initialization.chunkSize = 5;
		frameReader->setChannels(6);
		frameReader->setSampleRate(7);
		player.initialize(initialization);
		assertEqual("a", processorFactory.parameters().leftDslPrescriptionFilePath);
		assertEqual("b", processorFactory.parameters().rightDslPrescriptionFilePath);
		assertEqual("c", processorFactory.parameters().brirFilePath);
		EXPECT_EQ(1, processorFactory.parameters().max_dB_Spl);
		EXPECT_EQ(2, processorFactory.parameters().attack_ms);
		EXPECT_EQ(3, processorFactory.parameters().release_ms);
		EXPECT_EQ(4, processorFactory.parameters().windowSize);
		EXPECT_EQ(5, processorFactory.parameters().chunkSize);
		EXPECT_EQ(5U, device.streamParameters().framesPerBuffer);
		EXPECT_EQ(6, device.streamParameters().channels);
		EXPECT_EQ(7, device.streamParameters().sampleRate);
	}

	TEST_F(AudioPlayerTests, playPassesParametersToFactories) {
		StimulusPlayer::PlayRequest request;
		request.leftDslPrescriptionFilePath = "a";
		request.rightDslPrescriptionFilePath = "b";
		request.audioFilePath = "c";
		request.brirFilePath = "d";
		device.setDescriptions({ "alpha", "beta", "gamma", "lambda" });
		request.audioDevice = "gamma";
		request.max_dB_Spl = 1;
		request.attack_ms = 2;
		request.release_ms = 3;
		request.windowSize = 4;
		request.chunkSize = 5;
		frameReader->setChannels(6);
		frameReader->setSampleRate(7);
		play(request);
		assertEqual("a", processorFactory.parameters().leftDslPrescriptionFilePath);
		assertEqual("b", processorFactory.parameters().rightDslPrescriptionFilePath);
		assertEqual("c", readerFactory.filePath());
		assertEqual("d", processorFactory.parameters().brirFilePath);
		EXPECT_EQ(2, device.streamParameters().deviceIndex);
		EXPECT_EQ(1, processorFactory.parameters().max_dB_Spl);
		EXPECT_EQ(2, processorFactory.parameters().attack_ms);
		EXPECT_EQ(3, processorFactory.parameters().release_ms);
		EXPECT_EQ(4, processorFactory.parameters().windowSize);
		EXPECT_EQ(5, processorFactory.parameters().chunkSize);
		EXPECT_EQ(5U, device.streamParameters().framesPerBuffer);
		EXPECT_EQ(6, device.streamParameters().channels);
		EXPECT_EQ(7, device.streamParameters().sampleRate);
	}

	TEST_F(AudioPlayerTests, playSetsCallbackResultToContinueBeforeStartingStream) {
		play();
		assertEqual("setCallbackResultToContinue start ", device.callbackLog());
	}

	TEST_F(AudioPlayerTests, isPlayingWhenDeviceIsStreaming) {
		EXPECT_FALSE(player.isPlaying());
		device.setStreaming();
		EXPECT_TRUE(player.isPlaying());
	}

	TEST_F(AudioPlayerTests, audioDeviceDescriptionsReturnsDescriptions) {
		device.setDescriptions({ "a", "b", "c" });
		assertEqual({ "a", "b", "c" }, player.audioDeviceDescriptions());
	}

	class ReadsAOne : public AudioFrameReader {
		void read(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x = 1;
		}

		int channels() const override {
			return 1;
		}

		bool complete() const override { return {}; }
		int sampleRate() const override { return {}; }
		long long frames() const override { return {}; }
		void reset() override {}
	};

	class TimesTwo : public AudioFrameProcessor {
		void process(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x *= 2;
		}

		int groupDelay() override { return {}; }
	};

	TEST_F(AudioPlayerTests, fillBufferReadsThenProcesses) {
		readerFactory.setReader(std::make_shared<ReadsAOne>());
		processorFactory.setProcessor(std::make_shared<TimesTwo>());
		play();
		float x{};
		float *audio[] = { &x };
		fillStreamBuffer(audio, 1);
		EXPECT_EQ(2, x);
	}

	TEST_F(AudioPlayerTests, playPassesCalibrationScaleToProcessorFactory) {
		FakeAudioFileReader fake{ { 1, 2, 3, 4, 5, 6 } };
		fake.setChannels(2);
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fake));
		AudioPlayer::PlayRequest request{};
		request.level_dB_Spl = 7;
		request.max_dB_Spl = 8;
		play(request);
		assertEqual(
			{
				std::pow(10.0, (7 - 8) / 20.0) / std::sqrt((1 * 1 + 3 * 3 + 5 * 5) / 3.0),
				std::pow(10.0, (7 - 8) / 20.0) / std::sqrt((2 * 2 + 4 * 4 + 6 * 6) / 3.0)
			},
			processorFactory.parameters().channelScalars,
			1e-6
		);
	}

	TEST_F(AudioPlayerTests, playResetsReaderAfterComputingRms) {
		play();
		EXPECT_TRUE(frameReader->readingLog().endsWith("reset "));
	}

	class FailsToOpenStream : public AudioDevice {
		std::string errorMessage_{};
		bool failed_{};
	public:
		void setErrorMessage(std::string s) {
			errorMessage_ = std::move(s);
		}

		void openStream(StreamParameters) override {
			failed_ = true;
		}

		bool failed() override {
			return failed_;
		}

		std::string errorMessage() override {
			return errorMessage_;
		}

		void setController(AudioDeviceController *) override {}
		void startStream() override {}
		void stopStream() override {}
		bool streaming() const override { return {}; }
		void setCallbackResultToComplete() override {}
		void setCallbackResultToContinue() override {}
		void closeStream() override {}
		int count() override { return {}; }
		std::string description(int) override { return {}; }
	};

	TEST(
		DeviceFailureTests,
		constructorThrowsDeviceFailureWhenDeviceFailsToInitialize
	) {
		AudioDeviceStub device{};
		device.fail();
		device.setErrorMessage("error.");
		try {
			AudioPlayer player{ &device, {}, {} };
			FAIL() << "Expected AudioPlayer::DeviceFailure";
		}
		catch (const AudioPlayer::DeviceFailure &e) {
			assertEqual("error.", e.what());
		}
	}

	class RequestErrorTests : public ::testing::Test {
	protected:
		AudioDeviceStub defaultDevice{};
		AudioFrameReaderStubFactory defaultReaderFactory{};
		AudioFrameProcessorStubFactory defaultProcessorFactory{};
		AudioDevice *device{&defaultDevice};
		AudioFrameReaderFactory *readerFactory{&defaultReaderFactory};
		AudioFrameProcessorFactory *processorFactory{&defaultProcessorFactory};

		void assertPlayThrowsRequestFailure(std::string what) {
			AudioPlayer player{ device, readerFactory, processorFactory };
			try {
				player.play({});
				FAIL() << "Expected AudioPlayer::RequestFailure";
			}
			catch (const AudioPlayer::RequestFailure &e) {
				assertEqual(what, e.what());
			}
		}
	};

	TEST_F(
		RequestErrorTests,
		playThrowsDeviceFailureWhenDeviceFailsToOpenStream
	) {
		FailsToOpenStream failingDevice{};
		failingDevice.setErrorMessage("error.");
		device = &failingDevice;
		assertPlayThrowsRequestFailure("error.");
	}

	TEST_F(
		RequestErrorTests,
		playThrowsRequestFailureWhenReaderFactoryThrowsCreateError
	) {
		ErrorAudioFrameReaderFactory failingFactory{ "error." };
		readerFactory = &failingFactory;
		assertPlayThrowsRequestFailure("error.");
	}

	TEST_F(
		RequestErrorTests,
		playThrowsRequestFailureWhenProcessorFactoryThrowsCreateError
	) {
		ErrorAudioFrameProcessorFactory failingFactory{ "error." };
		processorFactory = &failingFactory;
		assertPlayThrowsRequestFailure("error.");
	}
}