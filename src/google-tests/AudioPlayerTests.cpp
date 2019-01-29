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

	TEST_F(AudioPlayerTests, prepareToPlayWhileStreamingDoesNotAlterStream) {
		device.setStreaming();
		prepareToPlay();
		EXPECT_TRUE(device.streamLog().empty());
	}

	TEST_F(AudioPlayerTests, prepareToPlayOpensStream) {
		AudioPlayer::Preparation p;
		p.framesPerBuffer = 2;
		p.channels = 3;
		p.sampleRate = 4;
		prepareToPlay(p);
		EXPECT_EQ(2U, device.streamParameters().framesPerBuffer);
		EXPECT_EQ(3, device.streamParameters().channels);
		EXPECT_EQ(4, device.streamParameters().sampleRate);
	}

	TEST_F(AudioPlayerTests, prepareToPlayFindsDeviceIndex) {
		AudioPlayer::Preparation p;
		device.setDescriptions({ "zeroth", "first", "second", "third" });
		p.audioDevice = "second";
		prepareToPlay(p);
		EXPECT_EQ(2, device.streamParameters().deviceIndex);
	}

	TEST_F(AudioPlayerTests, prepareToPlayResetsLoaderPriorToQueryingIt) {
		prepareToPlay();
		EXPECT_TRUE(loader.log().beginsWith("reset "));
	}

	TEST_F(AudioPlayerTests, fillStreamBufferSetsCallbackResultToCompleteWhenLoadingCompletes) {
		fillStreamBuffer();
		EXPECT_FALSE(device.complete());
		loader.setComplete();
		fillStreamBuffer();
		EXPECT_TRUE(device.complete());
	}

	TEST_F(AudioPlayerTests, fillStreamBufferLoadsEachAudioChannel) {
		AudioPlayer::Preparation request;
		request.channels = 2;
		prepareToPlay(request);
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

		void assertPrepareToPlayThrowsRequestFailure(std::string what) {
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
		prepareToPlayThrowsDeviceFailureWhenDeviceFailsToOpenStream
	) {
		FailsToOpenStream failingDevice{};
		failingDevice.setErrorMessage("error.");
		device = &failingDevice;
		assertPrepareToPlayThrowsRequestFailure("error.");
	}
}