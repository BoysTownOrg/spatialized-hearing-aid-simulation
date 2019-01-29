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
				prepareToPlay();
				FAIL() << "Expected AudioPlayer::PreparationFailure";
			}
			catch (const AudioPlayer::PreparationFailure &e) {
				assertEqual(std::move(errorMessage), e.what());
			}
		}

		void prepareToPlay(AudioPlayer::Preparation r = {}) {
			player.prepareToPlay(std::move(r));
		}

		void stop() {
			player.stop();
		}

		void fillStreamBuffer(void *x = {}, int n = {}) {
			device.fillStreamBuffer(x, n);
		}
	};

	TEST_F(AudioPlayerTests, constructorSetsItselfAsDeviceController) {
		EXPECT_EQ(&player, device.controller());
	}

	TEST_F(AudioPlayerTests, prepareToPlayFirstClosesThenOpensStream) {
		prepareToPlay();
		assertEqual("close open ", device.streamLog());
	}

	TEST_F(AudioPlayerTests, prepareToPlaySetsCallbackResultToContinue) {
		prepareToPlay();
		assertEqual("setCallbackResultToContinue ", device.callbackLog());
	}

	TEST_F(AudioPlayerTests, playStartsStream) {
		player.play();
		assertEqual("start ", device.streamLog());
	}

	TEST_F(AudioPlayerTests, stopStopsStream) {
		stop();
		assertEqual("stop ", device.streamLog());
	}

	TEST_F(AudioPlayerTests, playWhileStreamingDoesNotAlterStream) {
		device.setStreaming();
		prepareToPlay();
		EXPECT_TRUE(device.streamLog().empty());
	}

	TEST_F(AudioPlayerTests, playPreparesLoaderAndOpensStream) {
		StimulusPlayer::Preparation request;
		request.audioFilePath = "a";
		request.framesPerBuffer = 2;
		loader.setChannels(3);
		loader.setSampleRate(4);
		prepareToPlay(request);
		assertEqual("a", loader.preparation().audioFilePath);
		EXPECT_EQ(2U, device.streamParameters().framesPerBuffer);
		EXPECT_EQ(3, device.streamParameters().channels);
		EXPECT_EQ(4, device.streamParameters().sampleRate);
	}

	TEST_F(AudioPlayerTests, playFindsDeviceIndex) {
		StimulusPlayer::Preparation request;
		device.setDescriptions({ "zeroth", "first", "second", "third" });
		request.audioDevice = "second";
		prepareToPlay(request);
		EXPECT_EQ(2, device.streamParameters().deviceIndex);
	}

	TEST_F(AudioPlayerTests, playPreparesLoaderPriorToQueryingIt) {
		prepareToPlay();
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
		prepareToPlay();
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

		void assertPlayThrowsRequestFailure(std::string what) {
			auto player = makePlayer();
			try {
				player.prepareToPlay({});
				FAIL() << "Expected AudioPlayer::PreparationFailure";
			}
			catch (const AudioPlayer::PreparationFailure &e) {
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