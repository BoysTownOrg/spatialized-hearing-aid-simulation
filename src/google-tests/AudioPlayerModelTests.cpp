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

class MockSpatializedHearingAidSimulatorFactory : public SpatializedHearingAidSimulatorFactory {
	SpatializedHearingAidSimulator::Parameters _parameters{};
public:
	const SpatializedHearingAidSimulator::Parameters &parameters() const {
		return _parameters;
	}

	std::shared_ptr<SpatializedHearingAidSimulator> make(
		SpatializedHearingAidSimulator::Parameters p) override
	{
		_parameters = p;
		return {};
	}
};

class AudioPlayerModelFacade {
	PlayAudioModel model;
public:
	explicit AudioPlayerModelFacade(
		std::shared_ptr<AudioDeviceFactory> deviceFactory =
			std::make_shared<MockAudioDeviceFactory>(),
		std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory =
			std::make_shared<MockSpatializedHearingAidSimulatorFactory>()
	) :
		model{
			std::move(deviceFactory),
			std::move(simulatorFactory)
	} {}

	explicit AudioPlayerModelFacade(
		std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory
	) :
		AudioPlayerModelFacade{
			std::make_shared<MockAudioDeviceFactory>(),
			simulatorFactory
	} {}

	void playRequest(PlayAudioModel::PlayRequest r) {
		model.playRequest(r);
	}
};

static SpatializedHearingAidSimulationModel::PlayRequest validRequest() {
	SpatializedHearingAidSimulationModel::PlayRequest request;
	request.attack_ms = "0.0";
	request.release_ms = "0.0";
	request.level_dB_Spl = "0.0";
	request.windowSize = "0";
	request.chunkSize = "0";
	return request;
}

static void expectEqual(std::string expected, std::string actual) {
	EXPECT_EQ(expected, actual);
}

static void expectRequestTransformationYieldsFailure(
	std::function<SpatializedHearingAidSimulationModel::PlayRequest(
		SpatializedHearingAidSimulationModel::PlayRequest)> transformation,
	std::string message) 
{
	try {
		AudioPlayerModelFacade model{};
		model.playRequest(transformation(validRequest()));
		FAIL() << "Expected SpatializedHearingAidSimulationModel::RequestFailure";
	}
	catch (const SpatializedHearingAidSimulationModel::RequestFailure &failure) {
		expectEqual(message, failure.what());
	}
}

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, nonFloatsThrowRequestFailures) {
	expectRequestTransformationYieldsFailure(
		[](SpatializedHearingAidSimulationModel::PlayRequest request) { 
			request.level_dB_Spl = "a";
			return request;
		},
		"'a' is not a valid level.");
	expectRequestTransformationYieldsFailure(
		[](SpatializedHearingAidSimulationModel::PlayRequest request) {
			request.attack_ms = "a";
			return request;
		},
		"'a' is not a valid attack time.");
	expectRequestTransformationYieldsFailure(
		[](SpatializedHearingAidSimulationModel::PlayRequest request) {
			request.release_ms = "a";
			return request;
		},
		"'a' is not a valid release time.");
}

static void expectBadWindowSize(std::string size) {
	expectRequestTransformationYieldsFailure(
		[=](SpatializedHearingAidSimulationModel::PlayRequest request) {
			request.windowSize = size;
			return request;
		},
		"'" + size + "' is not a valid window size.");
}

static void expectBadChunkSize(std::string size) {
	expectRequestTransformationYieldsFailure(
		[=](SpatializedHearingAidSimulationModel::PlayRequest request) {
			request.chunkSize = size;
			return request;
		},
		"'" + size + "' is not a valid chunk size.");
}

TEST(AudioPlayerModelTestCase, nonPositiveIntegersThrowRequestFailures) {
	for (const auto s : std::vector<std::string>{ "a", "0.1", "-1" }) {
		expectBadWindowSize(s);
		expectBadChunkSize(s);
	}
}

TEST(AudioPlayerModelTestCase, playRequestPassesParametersToFactories) {
	const auto deviceFactory = std::make_shared<MockAudioDeviceFactory>();
	const auto simulatorFactory = std::make_shared<MockSpatializedHearingAidSimulatorFactory>();
	PlayAudioModel model{ deviceFactory, simulatorFactory };
	PlayAudioModel::PlayRequest request;
	request.leftDslPrescriptionFilePath = "a";
	request.rightDslPrescriptionFilePath = "b";
	request.audioFilePath = "c";
	request.brirFilePath = "d";
	request.level_dB_Spl = "1";
	request.attack_ms = "2";
	request.release_ms = "3";
	request.windowSize = "4";
	request.chunkSize = "5";
	model.playRequest(request);
	EXPECT_EQ("a", simulatorFactory->parameters().leftDslPrescriptionFilePath);
	EXPECT_EQ("b", simulatorFactory->parameters().rightDslPrescriptionFilePath);
	EXPECT_EQ("c", simulatorFactory->parameters().audioFilePath);
	EXPECT_EQ("d", simulatorFactory->parameters().brirFilePath);
	EXPECT_EQ(1, simulatorFactory->parameters().level_dB_Spl);
	EXPECT_EQ(2, simulatorFactory->parameters().attack_ms);
	EXPECT_EQ(3, simulatorFactory->parameters().release_ms);
	EXPECT_EQ(4, simulatorFactory->parameters().windowSize);
	EXPECT_EQ(5, simulatorFactory->parameters().chunkSize);
	EXPECT_EQ(5, deviceFactory->parameters().framesPerBuffer);
}

/*
TEST(AudioPlayerModelTestCase, successfulRequestPlaysPlayer) {
	const auto player = std::make_shared<MockAudioPlayer>();
	const auto factory = std::make_shared<MockAudioPlayerFactory>(player);
	PlayAudioModel model{ factory };
	model.playRequest(validRequest());
	EXPECT_TRUE(player->played());
}*/
