#include "AudioDeviceStub.h"
#include "AudioLoaderStub.h"
#include "assert-utility.h"
#include <playing-audio/AudioDevicePlayer.h>
#include <gtest/gtest.h>

namespace {
	class AudioDevicePlayerTests : public ::testing::Test {
	protected:
		AudioDeviceStub device{};
		std::shared_ptr<AudioLoaderStub> loader =
			std::make_shared<AudioLoaderStub>();
		AudioDevicePlayer player{ &device };

		AudioDevicePlayerTests() {
			player.setAudioLoader(loader);
		}

		auto prepareToPlay(AudioDevicePlayer::Preparation r = {}) {
			return player.prepareToPlay(std::move(r));
		}

		auto fillStreamBuffer(void *x = {}, int n = {}) {
			return device.fillStreamBuffer(x, n);
		}
	};

	TEST_F(AudioDevicePlayerTests, constructorSetsItselfAsDeviceController) {
		EXPECT_EQ(&player, device.controller());
	}

	TEST_F(AudioDevicePlayerTests, prepareToPlayFirstClosesThenOpensStream) {
		prepareToPlay();
		assertEqual("close open ", device.streamLog());
	}

	TEST_F(AudioDevicePlayerTests, prepareToPlaySetsCallbackResultToContinue) {
		prepareToPlay();
		assertTrue(device.setCallbackResultToContinueCalled());
	}

	TEST_F(AudioDevicePlayerTests, playStartsStream) {
		player.play();
		assertTrue(device.streamStarted());
	}

	TEST_F(AudioDevicePlayerTests, stopStopsStream) {
		player.stop();
		assertTrue(device.streamStopped());
	}

	TEST_F(AudioDevicePlayerTests, prepareToPlayWhileStreamingDoesNotAlterStream) {
		device.setStreaming();
		prepareToPlay();
		assertTrue(device.streamLog().empty());
	}

	TEST_F(AudioDevicePlayerTests, prepareToPlayPassesStreamParametersToDevice) {
		AudioDevicePlayer::Preparation p;
		p.framesPerBuffer = 2;
		p.channels = 3;
		p.sampleRate = 4;
		prepareToPlay(p);
		assertEqual(2UL, device.streamParameters().framesPerBuffer);
		assertEqual(3, device.streamParameters().channels);
		assertEqual(4, device.streamParameters().sampleRate);
	}

	TEST_F(AudioDevicePlayerTests, prepareToPlayFindsDeviceIndex) {
		AudioDevicePlayer::Preparation p;
		device.setDescriptions({ "zeroth", "first", "second", "third" });
		p.audioDevice = "second";
		prepareToPlay(p);
		assertEqual(2, device.streamParameters().deviceIndex);
	}

	TEST_F(AudioDevicePlayerTests, fillStreamBufferSetsCallbackResultToCompleteWhenLoadingCompletes) {
		fillStreamBuffer();
		assertFalse(device.complete());
		loader->setComplete();
		fillStreamBuffer();
		assertTrue(device.complete());
	}

	TEST_F(AudioDevicePlayerTests, fillStreamBufferLoadsEachAudioChannel) {
		AudioDevicePlayer::Preparation p;
		p.channels = 2;
		prepareToPlay(p);
		float left{};
		float right{};
		float *x[]{ &left, &right };
		fillStreamBuffer(x, 1);
		assertEqual(&left, loader->audioBuffer().at(0).data());
		assertEqual(&right, loader->audioBuffer().at(1).data());
		using size_type = typename gsl::span<float>::size_type;
		assertEqual(size_type{ 1 }, loader->audioBuffer().at(0).size());
		assertEqual(size_type{ 1 }, loader->audioBuffer().at(1).size());
	}

	TEST_F(AudioDevicePlayerTests, isPlayingWhenDeviceIsStreaming) {
		assertFalse(player.isPlaying());
		device.setStreaming();
		assertTrue(player.isPlaying());
	}

	TEST_F(AudioDevicePlayerTests, audioDeviceDescriptionsReturnsDescriptions) {
		device.setDescriptions({ "a", "b", "c" });
		assertEqual({ "a", "b", "c" }, player.audioDeviceDescriptions());
	}

	class AudioPlayerFailureTests : public ::testing::Test {
	protected:
		AudioDeviceStub defaultDevice{};
		AudioDevice *device{&defaultDevice};

		void assertConstructorThrowsDeviceFailure(std::string what) {
			try {
				auto player = makePlayer();
				FAIL() << "Expected AudioDevicePlayer::DeviceFailure";
			}
			catch (const AudioDevicePlayer::DeviceFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		AudioDevicePlayer makePlayer() {
			return AudioDevicePlayer{ device };
		}

		void assertPrepareToPlayThrowsPreparationFailure(std::string what) {
			auto player = makePlayer();
			try {
				player.prepareToPlay({});
				FAIL() << "Expected AudioDevicePlayer::PreparationFailure";
			}
			catch (const AudioDevicePlayer::PreparationFailure &e) {
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
