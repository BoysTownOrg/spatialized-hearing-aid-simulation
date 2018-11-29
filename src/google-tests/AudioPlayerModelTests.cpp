#include <playing-audio/PlayAudioModel.h>
#include <gtest/gtest.h>
#include <functional>

class MockAudioDeviceFactory : public AudioDeviceFactory {
	AudioDevice::Parameters _parameters{};
public:
	std::shared_ptr<AudioDevice> make(AudioDevice::Parameters p) override {
		_parameters = p;
		return std::shared_ptr<AudioDevice>();
	}
	const AudioDevice::Parameters &parameters() const {
		return _parameters;
	}
};

class AudioPlayerModelFacade {
	PlayAudioModel model;
public:
	explicit AudioPlayerModelFacade(
		std::shared_ptr<AudioDeviceFactory> deviceFactory =
			std::make_shared<MockAudioDeviceFactory>()
	) :
		model{ std::move(deviceFactory)} {}

	void playRequest(PlayAudioModel::PlayRequest r) {
		model.playRequest(r);
	}
};

/*
TEST(AudioPlayerModelTestCase, successfulRequestPlaysPlayer) {
	const auto player = std::make_shared<MockAudioPlayer>();
	const auto factory = std::make_shared<MockAudioPlayerFactory>(player);
	PlayAudioModel model{ factory };
	model.playRequest(validRequest());
	EXPECT_TRUE(player->played());
}*/


TEST(AudioPlayerModelTestCase, playRequestPassesParametersToFactories) {
	const auto deviceFactory = std::make_shared<MockAudioDeviceFactory>();
	PlayAudioModel model{ deviceFactory };
	PlayAudioModel::PlayRequest request;
	request.leftDslPrescriptionFilePath = "a";
	request.rightDslPrescriptionFilePath = "b";
	request.audioFilePath = "c";
	request.brirFilePath = "d";
	request.level_dB_Spl = 1;
	request.attack_ms = 2;
	request.release_ms = 3;
	request.windowSize = 4;
	request.chunkSize = 5;
	model.playRequest(request);
	EXPECT_EQ(5, deviceFactory->parameters().framesPerBuffer);
}
