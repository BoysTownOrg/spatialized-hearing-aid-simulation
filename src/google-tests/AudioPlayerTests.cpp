#include "AudioDeviceStub.h"
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
		std::shared_ptr<AudioFrameProcessorStub> obsolete_processor = std::make_shared<AudioFrameProcessorStub>();
		NoLongerFactoryStub noLongerFactory{ obsolete_processor };
		AudioPlayer player{ &device, &noLongerFactory };

		void assertPlayThrowsDeviceFailureWithMessage(std::string errorMessage) {
			try {
				play();
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

	TEST_F(AudioPlayerTests, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
		play();
		float left{};
		float *x[]{ &left };
		fillStreamBuffer(x, 1);
		EXPECT_FALSE(device.complete());
		noLongerFactory.setComplete();
		fillStreamBuffer(x, 1);
		EXPECT_TRUE(device.complete());
	}

	TEST_F(AudioPlayerTests, fillStreamBufferPassesAudio) {
		noLongerFactory.setChannels(2);
		play();
		float left{};
		float right{};
		float *x[]{ &left, &right };
		fillStreamBuffer(x, 1);
		EXPECT_EQ(&left, noLongerFactory.audioBuffer()[0].data());
		EXPECT_EQ(&right, noLongerFactory.audioBuffer()[1].data());
		EXPECT_EQ(1, noLongerFactory.audioBuffer()[0].size());
		EXPECT_EQ(1, noLongerFactory.audioBuffer()[1].size());
	}

	TEST_F(AudioPlayerTests, initializeInitializesProcessor) {
		StimulusPlayer::Initialization initialization;
		initialization.leftDslPrescriptionFilePath = "a";
		initialization.rightDslPrescriptionFilePath = "b";
		initialization.brirFilePath = "c";
		initialization.max_dB_Spl = 1;
		initialization.attack_ms = 2;
		initialization.release_ms = 3;
		initialization.windowSize = 4;
		initialization.chunkSize = 5;
		player.initialize(initialization);
		assertEqual("a", noLongerFactory.parameters().leftDslPrescriptionFilePath);
		assertEqual("b", noLongerFactory.parameters().rightDslPrescriptionFilePath);
		assertEqual("c", noLongerFactory.parameters().brirFilePath);
		EXPECT_EQ(1, noLongerFactory.parameters().max_dB_Spl);
		EXPECT_EQ(2, noLongerFactory.parameters().attack_ms);
		EXPECT_EQ(3, noLongerFactory.parameters().release_ms);
		EXPECT_EQ(4, noLongerFactory.parameters().windowSize);
		EXPECT_EQ(5, noLongerFactory.parameters().chunkSize);
	}

	TEST_F(AudioPlayerTests, playPassesParametersToFactories) {
		StimulusPlayer::Initialization initialization;
		initialization.chunkSize = 5;
		player.initialize(initialization);
		StimulusPlayer::PlayRequest request;
		request.audioFilePath = "d";
		device.setDescriptions({ "alpha", "beta", "gamma", "lambda" });
		request.audioDevice = "gamma";
		request.level_dB_Spl = 8;
		noLongerFactory.setChannels(6);
		noLongerFactory.setSampleRate(7);
		play(request);
		assertEqual("d", noLongerFactory.audioFilePath());
		EXPECT_EQ(8, noLongerFactory.parameters().level_dB_Spl);
		EXPECT_EQ(2, device.streamParameters().deviceIndex);
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
			AudioPlayer player{ &device, {} };
			FAIL() << "Expected AudioPlayer::DeviceFailure";
		}
		catch (const AudioPlayer::DeviceFailure &e) {
			assertEqual("error.", e.what());
		}
	}

	class RequestErrorTests : public ::testing::Test {
	protected:
		AudioDeviceStub defaultDevice{};
		NoLongerFactoryStub defaultNoLongerFactory{};
		AudioDevice *device{&defaultDevice};
		NoLongerFactory *noLongerFactory{&defaultNoLongerFactory};

		void assertPlayThrowsRequestFailure(std::string what) {
			AudioPlayer player{ device, noLongerFactory };
			try {
				player.play({});
				FAIL() << "Expected AudioPlayer::RequestFailure";
			}
			catch (const AudioPlayer::RequestFailure &e) {
				assertEqual(what, e.what());
			}
		}

		void assertInitializeThrowsInitializationFailure(std::string what) {
			AudioPlayer player{ device, noLongerFactory };
			try {
				player.initialize({});
				FAIL() << "Expected AudioPlayer::InitializationFailure";
			}
			catch (const AudioPlayer::InitializationFailure &e) {
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
		playThrowsRequestFailureWhenNoLongerFactoryThrowsCreateError
	) {
		ErrorNoLongerFactory failingFactory{ "error." };
		noLongerFactory = &failingFactory;
		assertPlayThrowsRequestFailure("error.");
	}

	TEST_F(
		RequestErrorTests,
		initializeThrowsInitializationFailureWhenNoLongerFactoryThrowsCreateError
	) {
		ErrorNoLongerFactory failingFactory{ "error." };
		noLongerFactory = &failingFactory;
		assertInitializeThrowsInitializationFailure("error.");
	}
}