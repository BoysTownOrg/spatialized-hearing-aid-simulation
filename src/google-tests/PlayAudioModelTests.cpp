#include "assert-utility.h"
#include "MockAudioFileReader.h"
#include "MockConfigurationFileParser.h"
#include "MockFilterbankCompressor.h"
#include "MockAudioDevice.h"
#include <playing-audio/PlayAudioModel.h>
#include <gtest/gtest.h>

class PlayAudioModelFacade {
	PlayAudioModel model;
public:
	explicit PlayAudioModelFacade(
		std::shared_ptr<AudioDevice> device =
			std::make_shared<MockAudioDevice>(),
		std::shared_ptr<ConfigurationFileParserFactory> parserFactory =
			std::make_shared<MockParserFactory>(),
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory =
			std::make_shared<MockCompressorFactory>(),
		std::shared_ptr<AudioFileReaderFactory> audioFileFactory =
			std::make_shared<MockAudioFileReaderFactory>()
	) :
		model{ 
			std::move(device),
			std::move(compressorFactory),
			std::move(audioFileFactory),
			std::move(parserFactory)
		} {}

	static std::shared_ptr<PlayAudioModelFacade> withValidParser(std::shared_ptr<AudioDevice> device) {
		const auto parser = std::make_shared<MockConfigurationFileParser>();
		parser->setValidSingleChannelDslProperties();
		parser->setValidBrirProperties();
		return std::make_shared<PlayAudioModelFacade>(
			device,
			std::make_shared<MockParserFactory>(parser)
		);
	}

	void playRequest(PlayAudioModel::PlayRequest r) {
		model.playRequest(r);
	}

	const PlayAudioModel *get() const {
		return &model;
	}
};

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, constructorSetsItself) {
	const auto device = std::make_shared<MockAudioDevice>();
	auto model = PlayAudioModelFacade::withValidParser(device);
	EXPECT_EQ(model->get(), device->controller());
}

static void assertDeviceFailureOnConstruction(
	std::shared_ptr<AudioDevice> device, 
	std::string message
) {
	try {
		PlayAudioModelFacade model{ device };
		FAIL() << "Expected PlayAudioModel::DeviceFailure";
	}
	catch (const PlayAudioModel::DeviceFailure &e) {
		assertEqual(message, e.what());
	}
}

TEST(
	AudioPlayerModelTestCase,
	constructorThrowsDeviceFailureWhenDeviceFailsToInitialize)
{
	const auto device = std::make_shared<MockAudioDevice>();
	device->setFailedTrue();
	device->setErrorMessage("error.");
	assertDeviceFailureOnConstruction(device, "error.");
}

TEST(
	AudioPlayerModelTestCase,
	constructorThrowsDeviceFailureWhenDeviceDoesNotSupportAsio)
{
	const auto device = std::make_shared<MockAudioDevice>();
	device->setSupportsAsioFalse();
	assertDeviceFailureOnConstruction(device, "This device does not support ASIO.");
}

TEST(AudioPlayerModelTestCase, playRequestFirstClosesStreamThenOpensThenStarts) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto model = PlayAudioModelFacade::withValidParser(device);
	model->playRequest({});
	assertEqual("close open start ", device->streamLog());
}

TEST(
	AudioPlayerModelTestCase,
	playRequestThrowsRequestErrorWhenDeviceFailure)
{
	try {
		const auto device = std::make_shared<MockAudioDevice>();
		const auto model = PlayAudioModelFacade::withValidParser(device);
		device->setFailedTrue();
		device->setErrorMessage("error.");
		model->playRequest({});
		FAIL() << "Expected PlayAudioModel::RequestFailure";
	}
	catch (const PlayAudioModel::RequestFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(AudioPlayerModelTestCase, playRequestWhileStreamingDoesNotAlterCurrentStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto model = PlayAudioModelFacade::withValidParser(device);
	device->setStreaming();
	model->playRequest({});
	EXPECT_TRUE(device->streamLog().empty());
}

TEST(AudioPlayerModelTestCase, playRequestPassesParametersToFactories) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto reader = std::make_shared<MockAudioFileReader>();
	reader->setSampleRate(48000);
	const auto parser = std::make_shared<MockConfigurationFileParser>();
	parser->setValidSingleChannelDslProperties();
	parser->setValidBrirProperties();
	parser->setIntProperty(propertyName(binaural_room_impulse_response::Property::sampleRate), 48000);
	const auto compressorFactory = std::make_shared<MockCompressorFactory>();
	const auto audioFactory = std::make_shared<MockAudioFileReaderFactory>(reader);
	PlayAudioModelFacade model{
		device,
		std::make_shared<MockParserFactory>(parser),
		compressorFactory,
		audioFactory
	};
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
	EXPECT_EQ("c", audioFactory->filePath());
	EXPECT_EQ(2, compressorFactory->parameters().attack_ms);
	EXPECT_EQ(3, compressorFactory->parameters().release_ms);
	EXPECT_EQ(4, compressorFactory->parameters().windowSize);
	EXPECT_EQ(5, compressorFactory->parameters().chunkSize);
	EXPECT_EQ(5, device->streamParameters().framesPerBuffer);
	EXPECT_EQ(48000, device->streamParameters().sampleRate);
	assertEqual({ 0, 1 }, device->streamParameters().channels);
}

TEST(AudioPlayerModelTestCase, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
	const auto device = std::make_shared<MockAudioDevice>();
	auto model = PlayAudioModelFacade::withValidParser(device);
	model->playRequest({});
	float left{};
	float right{};
	float *x[]{ &left, &right };
	device->fillStreamBuffer(x, 0);
	EXPECT_TRUE(device->setCallbackResultToCompleteCalled());
}

TEST(AudioPlayerModelTestCase, playRequestSetsCallbackResultToContinue) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto model = PlayAudioModelFacade::withValidParser(device);
	model->playRequest({});
	EXPECT_TRUE(device->setCallbackResultToContinueCalled());
}

/*
TEST(AudioPlayerModelTestCase, fillStreamBufferFillsFromStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto stream = std::make_shared<MockAudioFrameReader>();
	AudioDeviceController model{ device, stream };
	float *channel{};
	device->fillStreamBuffer(&channel, 1);
	EXPECT_EQ(&channel, stream->channels());
	EXPECT_EQ(1, stream->frameCount());
}
*/
