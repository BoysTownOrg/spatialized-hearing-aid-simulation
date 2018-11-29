#include "MockAudioFileReader.h"
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

class MockCompressorFactory : public FilterbankCompressorFactory {
	std::shared_ptr<FilterbankCompressor> make(
		const DslPrescription &, 
		FilterbankCompressor::Parameters) override
	{
		return std::shared_ptr<FilterbankCompressor>();
	}
};

class MockConfigurationParser : public ConfigurationFileParser {
	std::vector<double> asVector(std::string property) const override
	{
		return { 0 };
	}
	double asDouble(std::string property) const override
	{
		return 0.0;
	}
	int asInt(std::string property) const override
	{
		return 0;
	}
};

class MockParserFactory : public ConfigurationFileParserFactory {
	std::shared_ptr<ConfigurationFileParser> parser;
public:
	explicit MockParserFactory(
		std::shared_ptr<ConfigurationFileParser> parser =
			std::make_shared<MockConfigurationParser>()
	) :
		parser{ std::move(parser) } {}

	std::shared_ptr<ConfigurationFileParser> make(std::string filePath) override
	{
		return parser;
	}
};

class MockAudioFileFactory : public AudioFileReaderFactory {
	std::shared_ptr<AudioFileReader> reader;
public:
	explicit MockAudioFileFactory(
		std::shared_ptr<AudioFileReader> reader =
			std::make_shared<MockAudioFileReader>()
	) :
		reader{ std::move(reader) } {}

	std::shared_ptr<AudioFileReader> make(std::string filePath) override
	{
		return reader;
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
			std::make_shared<MockAudioFileFactory>(),
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
	PlayAudioModelFacade model{ 
		deviceFactory, 
		std::make_shared<MockCompressorFactory>(),
		std::make_shared<MockAudioFileFactory>(reader) };
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
