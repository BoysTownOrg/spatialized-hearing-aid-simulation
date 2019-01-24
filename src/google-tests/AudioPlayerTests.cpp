#include "AudioDeviceStub.h"
#include "AudioLoaderStub.h"
#include "assert-utility.h"
#include <playing-audio/AudioPlayer.h>
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
	class AudioPlayerTests : public ::testing::Test {
	protected:
		AudioDeviceStub device{};
		AudioLoaderStub loader{};
		AudioPlayer player{ &device, &loader };

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

	TEST_F(AudioPlayerTests, initializeInitializesLoader) {
		StimulusPlayer::Initialization init;
		GlobalTestParameters global;
		init.global = &global;
		player.initialize(init);
		EXPECT_EQ(&global, loader.initialization().global);
	}

	TEST_F(AudioPlayerTests, playClosesOpensAndStartsStreamInOrder) {
		play();
		assertEqual("close open start ", device.streamLog());
	}

	TEST_F(AudioPlayerTests, playWhileStreamingDoesNotAlterStream) {
		device.setStreaming();
		play();
		EXPECT_TRUE(device.streamLog().empty());
	}

	TEST_F(AudioPlayerTests, playPreparesLoaderAndOpensStream) {
		StimulusPlayer::PlayRequest request;
		request.audioFilePath = "a";
		request.level_dB_Spl = 1;
		loader.setBufferSize(2);
		loader.setChannels(3);
		loader.setSampleRate(4);
		play(request);
		assertEqual("a", loader.preparation().audioFilePath);
		EXPECT_EQ(1, loader.preparation().level_dB_Spl);
		EXPECT_EQ(2U, device.streamParameters().framesPerBuffer);
		EXPECT_EQ(3, device.streamParameters().channels);
		EXPECT_EQ(4, device.streamParameters().sampleRate);
	}

	TEST_F(AudioPlayerTests, playFindsDeviceIndex) {
		StimulusPlayer::PlayRequest request;
		device.setDescriptions({ "zeroth", "first", "second", "third" });
		request.audioDevice = "second";
		play(request);
		EXPECT_EQ(2, device.streamParameters().deviceIndex);
	}

	TEST_F(AudioPlayerTests, playSetsCallbackResultToContinueBeforeStartingStream) {
		play();
		assertEqual("setCallbackResultToContinue start ", device.callbackLog());
	}

	TEST_F(AudioPlayerTests, playPreparesLoaderPriorToQueryingIt) {
		play();
		EXPECT_TRUE(loader.log().beginsWith("prepare "));
	}

	TEST_F(AudioPlayerTests, fillStreamBufferSetsCallbackResultToCompleteWhenLoadingCompletes) {
		fillStreamBuffer();
		EXPECT_FALSE(device.complete());
		loader.setComplete();
		fillStreamBuffer();
		EXPECT_TRUE(device.complete());
	}

	TEST_F(AudioPlayerTests, fillStreamBufferLoadsEachAudioChannel) {
		loader.setChannels(2);
		play();
		float left{};
		float right{};
		float *x[]{ &left, &right };
		fillStreamBuffer(x, 1);
		EXPECT_EQ(&left, loader.audioBuffer().at(0).data());
		EXPECT_EQ(&right, loader.audioBuffer().at(1).data());
		EXPECT_EQ(1, loader.audioBuffer().at(0).size());
		EXPECT_EQ(1, loader.audioBuffer().at(1).size());
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

	class AudioPlayerFailureTests : public ::testing::Test {
	protected:
		AudioDeviceStub defaultDevice{};
		AudioLoaderStub defaultLoader{};
		AudioDevice *device{&defaultDevice};
		AudioLoader *loader{&defaultLoader};

		void assertConstructorThrowsDeviceFailure(std::string what) {
			try {
				makePlayer();
				FAIL() << "Expected AudioPlayer::DeviceFailure";
			}
			catch (const AudioPlayer::DeviceFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		AudioPlayer makePlayer() {
			return { device, loader };
		}

		void assertInitializeThrowsInitializationFailure(std::string what) {
			auto player = makePlayer();
			try {
				player.initialize({});
				FAIL() << "Expected AudioPlayer::InitializationFailure";
			}
			catch (const AudioPlayer::InitializationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertPlayThrowsRequestFailure(std::string what) {
			auto player = makePlayer();
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
		AudioPlayerFailureTests,
		constructorThrowsDeviceFailureWhenDeviceFailsToInitialize
	) {
		AudioDeviceStub failingDevice{};
		failingDevice.fail();
		failingDevice.setErrorMessage("error.");
		device = &failingDevice;
		assertConstructorThrowsDeviceFailure("error.");
	}

	TEST_F(
		AudioPlayerFailureTests,
		initializeThrowsInitializationFailureWhenAudioLoaderThrowsInitializationFailure
	) {
		InitializationFailingAudioLoader failingFactory{ "error." };
		loader = &failingFactory;
		assertInitializeThrowsInitializationFailure("error.");
	}

	TEST_F(
		AudioPlayerFailureTests,
		playThrowsDeviceFailureWhenDeviceFailsToOpenStream
	) {
		FailsToOpenStream failingDevice{};
		failingDevice.setErrorMessage("error.");
		device = &failingDevice;
		assertPlayThrowsRequestFailure("error.");
	}

	TEST_F(
		AudioPlayerFailureTests,
		playThrowsRequestFailureWhenAudioLoaderThrowsPreparationFailure
	) {
		PreparationFailureAudioLoader failingFactory{ "error." };
		loader = &failingFactory;
		assertPlayThrowsRequestFailure("error.");
	}
}