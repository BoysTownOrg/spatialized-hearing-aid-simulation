#include "AudioDeviceStub.h"
#include "AudioLoaderStub.h"
#include "assert-utility.h"
#include <playing-audio/AudioPlayer.h>
#include <gtest/gtest.h>

namespace {
	class AudioPlayerTests : public ::testing::Test {
	protected:
		AudioDeviceStub device{};
		AudioLoaderStub loader{};
		AudioPlayer player{ &device };

		AudioPlayerTests() {
			player.setAudioLoader(&loader);
		}

		auto prepareToPlay(AudioPlayer::Preparation r = {}) {
			return player.prepareToPlay(std::move(r));
		}

		auto fillStreamBuffer(void *x = {}, int n = {}) {
			return device.fillStreamBuffer(x, n);
		}

		void assertPlayThrowsDeviceFailureWithMessage(std::string errorMessage) {
			try {
				prepareToPlay();
				FAIL() << "Expected AudioPlayer::PreparationFailure";
			}
			catch (const AudioPlayer::PreparationFailure &e) {
				assertEqual(std::move(errorMessage), e.what());
			}
		}
	};

	TEST_F(AudioPlayerTests, constructorSetsItselfAsDeviceController) {
		EXPECT_EQ(
			&player, 
			device.controller()
		);
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
		player.stop();
		assertEqual("stop ", device.streamLog());
	}

	TEST_F(AudioPlayerTests, prepareToPlayWhileStreamingDoesNotAlterStream) {
		device.setStreaming();
		prepareToPlay();
		assertTrue(device.streamLog().empty());
	}

	TEST_F(AudioPlayerTests, prepareToPlayPassesStreamParametersToDevice) {
		AudioPlayer::Preparation p;
		p.framesPerBuffer = 2;
		p.channels = 3;
		p.sampleRate = 4;
		prepareToPlay(p);
		assertEqual(2UL, device.streamParameters().framesPerBuffer);
		assertEqual(3, device.streamParameters().channels);
		assertEqual(4, device.streamParameters().sampleRate);
	}

	TEST_F(AudioPlayerTests, prepareToPlayFindsDeviceIndex) {
		AudioPlayer::Preparation p;
		device.setDescriptions({ "zeroth", "first", "second", "third" });
		p.audioDevice = "second";
		prepareToPlay(p);
		assertEqual(2, device.streamParameters().deviceIndex);
	}

	TEST_F(AudioPlayerTests, fillStreamBufferSetsCallbackResultToCompleteWhenLoadingCompletes) {
		fillStreamBuffer();
		assertFalse(device.complete());
		loader.setComplete();
		fillStreamBuffer();
		assertTrue(device.complete());
	}

	TEST_F(AudioPlayerTests, fillStreamBufferLoadsEachAudioChannel) {
		AudioPlayer::Preparation p;
		p.channels = 2;
		prepareToPlay(p);
		float left{};
		float right{};
		float *x[]{ &left, &right };
		fillStreamBuffer(x, 1);
		assertEqual(&left, loader.audioBuffer().at(0).data());
		assertEqual(&right, loader.audioBuffer().at(1).data());
		assertEqual(1, loader.audioBuffer().at(0).size());
		assertEqual(1, loader.audioBuffer().at(1).size());
	}

	TEST_F(AudioPlayerTests, isPlayingWhenDeviceIsStreaming) {
		assertFalse(player.isPlaying());
		device.setStreaming();
		assertTrue(player.isPlaying());
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
				auto player = makePlayer();
				FAIL() << "Expected AudioPlayer::DeviceFailure";
			}
			catch (const AudioPlayer::DeviceFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		AudioPlayer makePlayer() {
			AudioPlayer player{ device };
			player.setAudioLoader(loader);
			return player;
		}

		void assertPrepareToPlayThrowsPreparationFailure(std::string what) {
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
		prepareToPlayThrowsPreparationFailureWhenDeviceFailsToOpenStream
	) {
		FailsToOpenStream failingDevice{};
		failingDevice.setErrorMessage("error.");
		device = &failingDevice;
		assertPrepareToPlayThrowsPreparationFailure("error.");
	}
}