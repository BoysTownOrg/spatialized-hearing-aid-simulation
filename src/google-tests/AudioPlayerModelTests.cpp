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
		std::shared_ptr<AudioDeviceFactory> deviceFactory =
			std::make_shared<MockAudioDeviceFactory>(),
		std::shared_ptr<ConfigurationFileParserFactory> parserFactory =
			std::make_shared<MockParserFactory>(),
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory =
			std::make_shared<MockCompressorFactory>(),
		std::shared_ptr<AudioFileReaderFactory> audioFileFactory =
			std::make_shared<MockAudioFileReaderFactory>()
	) :
		model{ 
			std::move(deviceFactory),
			std::move(compressorFactory),
			std::move(audioFileFactory),
			std::move(parserFactory)
		} {}

	void playRequest(PlayAudioModel::PlayRequest r) {
		model.playRequest(r);
	}
};

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, playRequestPassesParametersToFactories) {
	const auto deviceFactory = std::make_shared<MockAudioDeviceFactory>();
	const auto reader = std::make_shared<MockAudioFileReader>();
	reader->setSampleRate(48000);
	const auto parser = std::make_shared<MockConfigurationFileParser>();
	parser->setValidSingleChannelDslProperties();
	parser->setValidBrirProperties();
	parser->setIntProperty(propertyName(brir_config::Property::sampleRate), 48000);
	const auto compressorFactory = std::make_shared<MockCompressorFactory>();
	const auto audioFactory = std::make_shared<MockAudioFileReaderFactory>(reader);
	PlayAudioModelFacade model{ 
		deviceFactory,
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
	EXPECT_EQ(5, deviceFactory->parameters().framesPerBuffer);
	EXPECT_EQ(48000, deviceFactory->parameters().sampleRate);
	assertEqual({ 0, 1 }, deviceFactory->parameters().channels);
}

TEST(AudioPlayerModelTestCase, playRequestDoesNotDestroyStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto parser = std::make_shared<MockConfigurationFileParser>();
	parser->setValidSingleChannelDslProperties();
	parser->setValidBrirProperties();
	PlayAudioModelFacade model{
		std::make_shared<MockAudioDeviceFactory>(device),
		std::make_shared<MockParserFactory>(parser)
	};
	model.playRequest({});
	auto inController = 1;
	auto inFactory = 1;
	auto inTest = 1;
	EXPECT_EQ(inController + inFactory + inTest, device.use_count());
}

TEST(AudioPlayerModelTestCase, playRequestWhileStreamingDoesNotCreateNewStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto deviceFactory = std::make_shared<MockAudioDeviceFactory>(device);
	const auto parser = std::make_shared<MockConfigurationFileParser>();
	parser->setValidSingleChannelDslProperties();
	parser->setValidBrirProperties();
	PlayAudioModelFacade model{
		deviceFactory,
		std::make_shared<MockParserFactory>(parser)
	};
	model.playRequest({});
	model.playRequest({});
	EXPECT_EQ(1, deviceFactory->makeCalls());
}
