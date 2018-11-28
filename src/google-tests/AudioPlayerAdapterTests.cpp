#include <playing-audio/AudioPlayerAdapter.h>
#include <gtest/gtest.h>

class MockAudioDeviceFactory : public AudioDeviceFactory {
	AudioDevice::Parameters _parameters{};
public:
	std::shared_ptr<AudioDevice> make(AudioDevice::Parameters p) override {
		_parameters = p;
		return std::shared_ptr<AudioDevice>();
	}
	int framesPerBuffer() const {
		return _parameters.framesPerBuffer;
	}
	int sampleRate() const {
		return _parameters.sampleRate;
	}
};

class MockSpatializedHearingAidSimulatorFactory : public SpatializedHearingAidSimulatorFactory {
	SpatializedHearingAidSimulator::Parameters _parameters{};
	double _attack_ms{};
public:
	double attack_ms() const {
		return _parameters.attack_ms;
	}
	std::shared_ptr<SpatializedHearingAidSimulator> make(
		SpatializedHearingAidSimulator::Parameters p) override 
	{
		_parameters = p;
		return {};
	}
};

class AudioPlayerAdapterFactoryFacade {
	AudioPlayerAdapterFactory factory;
public:
	explicit AudioPlayerAdapterFactoryFacade(
		std::shared_ptr<AudioDeviceFactory> deviceFactory,
		std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory =
			std::make_shared<MockSpatializedHearingAidSimulatorFactory>()
	) :
		factory { 
			std::move(deviceFactory),
			std::move(simulatorFactory)
		} {}

	explicit AudioPlayerAdapterFactoryFacade(
		std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory
	) :
		AudioPlayerAdapterFactoryFacade{ 
			std::make_shared<MockAudioDeviceFactory>(),
			simulatorFactory
		} {}

	std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters p) {
		return factory.make(p);
	}
};

class AudioPlayerAdapterTestCase : public ::testing::TestCase {};

TEST(AudioPlayerAdapterTestCase, makePassesDeviceParametersToFactory) {
	const auto factory = std::make_shared<MockAudioDeviceFactory>();
	AudioPlayerAdapterFactoryFacade adapter{ factory };
	AudioPlayer::Parameters p{};
	p.forAudioDevice.framesPerBuffer = 1;
	p.forAudioDevice.sampleRate = 2;
	adapter.make(p);
	EXPECT_EQ(1, factory->framesPerBuffer());
	EXPECT_EQ(2, factory->sampleRate());
}

TEST(AudioPlayerAdapterTestCase, makePassesSimulationParametersToFactory) {
	const auto factory = std::make_shared<MockSpatializedHearingAidSimulatorFactory>();
	AudioPlayerAdapterFactoryFacade adapter{ factory };
	AudioPlayer::Parameters p{};
	p.forHearingAidSimulation.attack_ms = 1;
	adapter.make(p);
	EXPECT_EQ(1, factory->attack_ms());
}