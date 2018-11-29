#include "MockAudioFileReader.h"
#include "MockConfigurationFileParser.h"
#include "MockFilterbankCompressor.h"
#include "MockAudioDevice.h"
#include <playing-audio/PlayAudioModel.h>
#include <gtest/gtest.h>

class MockAudioDeviceFactory : public AudioDeviceFactory {
	AudioDevice::Parameters _parameters{};
	std::shared_ptr<AudioDevice> device;
public:
	explicit MockAudioDeviceFactory(
		std::shared_ptr<AudioDevice> device =
			std::make_shared<MockAudioDevice>()
	) :
		device{ std::move(device) } {}

	std::shared_ptr<AudioDevice> make(AudioDevice::Parameters p) override {
		_parameters = p;
		return device;
	}
	const AudioDevice::Parameters &parameters() const {
		return _parameters;
	}
};

class MockCompressorFactory : public FilterbankCompressorFactory {
	FilterbankCompressor::Parameters _parameters{};
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	explicit MockCompressorFactory(
		std::shared_ptr<FilterbankCompressor> compressor =
			std::make_shared<MockFilterbankCompressor>()
	) :
		compressor{ std::move(compressor) } {}

	const FilterbankCompressor::Parameters &parameters() const {
		return _parameters;
	}

	std::shared_ptr<FilterbankCompressor> make(
		const DslPrescription &, 
		FilterbankCompressor::Parameters p) override
	{
		_parameters = p;
		return compressor;
	}
};

class MockParserFactory : public ConfigurationFileParserFactory {
	std::shared_ptr<ConfigurationFileParser> parser;
public:
	explicit MockParserFactory(
		std::shared_ptr<ConfigurationFileParser> parser =
			std::make_shared<MockConfigurationFileParser>()
	) :
		parser{ std::move(parser) } {}

	std::shared_ptr<ConfigurationFileParser> make(std::string filePath) override
	{
		return parser;
	}
};

class PlayAudioModelFacade {
	PlayAudioModel model;
public:
	explicit PlayAudioModelFacade(
		std::shared_ptr<AudioDeviceFactory> deviceFactory =
			std::make_shared<MockAudioDeviceFactory>(),
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory =
			std::make_shared<MockCompressorFactory>(),
		std::shared_ptr<AudioFileReaderFactory> audioFileFactory =
			std::make_shared<MockAudioFileReaderFactory>(),
		std::shared_ptr<ConfigurationFileParserFactory> parserFactory =
			std::make_shared<MockParserFactory>()
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

TEST(AudioPlayerModelTestCase, playRequestPassesParametersToFactories) {
	const auto deviceFactory = std::make_shared<MockAudioDeviceFactory>();
	const auto reader = std::make_shared<MockAudioFileReader>();
	reader->setChannels(2);
	const auto parser = std::make_shared<MockConfigurationFileParser>();
	parser->setValidSingleChannelDslProperties();
	parser->setValidBrirProperties();
	const auto compressorFactory = std::make_shared<MockCompressorFactory>();
	const auto audioFactory = std::make_shared<MockAudioFileReaderFactory>(reader);
	PlayAudioModelFacade model{ 
		deviceFactory, 
		compressorFactory,
		audioFactory,
		std::make_shared<MockParserFactory>(parser)
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
}
