#include "AudioDeviceStub.h"
#include "AudioProcessorStub.h"
#include "assert-utility.h"
#include <playing-audio/AudioPlayer.h>
#include <gtest/gtest.h>

namespace {
	class AudioPlayerTests : public ::testing::Test {
	protected:
		AudioDeviceStub device{};
		AudioProcessorStub processor{};
		AudioPlayer player{ &device, &processor };

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
		fillStreamBuffer();
		EXPECT_FALSE(device.complete());
		processor.setComplete();
		fillStreamBuffer();
		EXPECT_TRUE(device.complete());
	}

	TEST_F(AudioPlayerTests, fillStreamBufferPassesAudio) {
		processor.setChannels(2);
		play();
		float left{};
		float right{};
		float *x[]{ &left, &right };
		fillStreamBuffer(x, 1);
		EXPECT_EQ(&left, processor.audioBuffer()[0].data());
		EXPECT_EQ(&right, processor.audioBuffer()[1].data());
		EXPECT_EQ(1, processor.audioBuffer()[0].size());
		EXPECT_EQ(1, processor.audioBuffer()[1].size());
	}

	TEST_F(AudioPlayerTests, initializeInitializesProcessor) {
		StimulusPlayer::Initialization init;
		init.leftDslPrescriptionFilePath = "a";
		init.rightDslPrescriptionFilePath = "b";
		init.brirFilePath = "c";
		init.max_dB_Spl = 1;
		init.attack_ms = 2;
		init.release_ms = 3;
		init.windowSize = 4;
		init.chunkSize = 5;
		player.initialize(init);
		assertEqual("a", processor.initialization().leftDslPrescriptionFilePath);
		assertEqual("b", processor.initialization().rightDslPrescriptionFilePath);
		assertEqual("c", processor.initialization().brirFilePath);
		EXPECT_EQ(1, processor.initialization().max_dB_Spl);
		EXPECT_EQ(2, processor.initialization().attack_ms);
		EXPECT_EQ(3, processor.initialization().release_ms);
		EXPECT_EQ(4, processor.initialization().windowSize);
		EXPECT_EQ(5, processor.initialization().chunkSize);
	}

	TEST_F(AudioPlayerTests, playPassesParametersToThings) {
		StimulusPlayer::Initialization init;
		init.chunkSize = 5;
		player.initialize(init);
		StimulusPlayer::PlayRequest request;
		request.audioFilePath = "d";
		device.setDescriptions({ "alpha", "beta", "gamma", "lambda" });
		request.audioDevice = "gamma";
		request.level_dB_Spl = 8;
		processor.setChannels(6);
		processor.setSampleRate(7);
		play(request);
		assertEqual("d", processor.preparation().audioFilePath);
		EXPECT_EQ(8, processor.preparation().level_dB_Spl);
		EXPECT_EQ(2, device.streamParameters().deviceIndex);
		EXPECT_EQ(5U, device.streamParameters().framesPerBuffer);
		EXPECT_EQ(6, device.streamParameters().channels);
		EXPECT_EQ(7, device.streamParameters().sampleRate);
	}

	TEST_F(AudioPlayerTests, playSetsCallbackResultToContinueBeforeStartingStream) {
		play();
		assertEqual("setCallbackResultToContinue start ", device.callbackLog());
	}

	TEST_F(AudioPlayerTests, playPreparesProcessorPriorToQueryingIt) {
		play();
		EXPECT_TRUE(processor.log().beginsWith("prepare "));
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
		AudioProcessorStub defaultProcessor{};
		AudioDevice *device{&defaultDevice};
		AudioProcessor *processor{&defaultProcessor};

		void assertInitializeThrowsInitializationFailure(std::string what) {
			AudioPlayer player{ device, processor };
			try {
				player.initialize({});
				FAIL() << "Expected AudioPlayer::InitializationFailure";
			}
			catch (const AudioPlayer::InitializationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayThrowsRequestFailure(std::string what) {
			AudioPlayer player{ device, processor };
			try {
				player.play({});
				FAIL() << "Expected AudioPlayer::RequestFailure";
			}
			catch (const AudioPlayer::RequestFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
	};

	TEST_F(
		RequestErrorTests,
		initializeThrowsInitializationFailureWhenAudioProcessorThrowsInitializationFailure
	) {
		InitializationFailingAudioProcessor failingFactory{ "error." };
		processor = &failingFactory;
		assertInitializeThrowsInitializationFailure("error.");
	}

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
		playThrowsRequestFailureWhenAudioProcessorThrowsPreparationFailure
	) {
		PreparationFailureAudioProcessor failingFactory{ "error." };
		processor = &failingFactory;
		assertPlayThrowsRequestFailure("error.");
	}
}