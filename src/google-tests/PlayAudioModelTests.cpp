#include "assert-utility.h"
#include "AudioFileReaderStub.h"
#include "FakeConfigurationFileParser.h"
#include "FilterbankCompressorSpy.h"
#include "AudioDeviceStub.h"
#include <playing-audio/PlayAudioModel.h>
#include <gtest/gtest.h>

class PlayAudioModelFacade {
	PlayAudioModel model;
public:
	explicit PlayAudioModelFacade(
		std::shared_ptr<AudioDevice> device =
			std::make_shared<AudioDeviceStub>(),
		std::shared_ptr<ConfigurationFileParserFactory> parserFactory =
			std::make_shared<MockParserFactory>(),
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory =
			std::make_shared<MockCompressorFactory>(),
		std::shared_ptr<AudioFileReaderFactory> audioFileFactory =
			std::make_shared<AudioFileReaderStubFactory>()
	) :
		model{ 
			std::move(device),
			std::move(compressorFactory),
			std::move(audioFileFactory),
			std::move(parserFactory)
		} {}

	static std::shared_ptr<PlayAudioModelFacade> withValidParser(std::shared_ptr<AudioDevice> device) {
		const auto parser = std::make_shared<FakeConfigurationFileParser>();
		parser->setValidSingleChannelDslProperties();
		parser->setValidBrirProperties();
		return std::make_shared<PlayAudioModelFacade>(
			device,
			std::make_shared<MockParserFactory>(parser)
		);
	}

	void play(PlayAudioModel::PlayRequest r) {
		model.play(r);
	}

	const PlayAudioModel *get() const {
		return &model;
	}

	std::vector<std::string> audioDeviceDescriptions() {
		return model.audioDeviceDescriptions();
	}
};

class PlayAudioModelTestCase : public ::testing::TestCase {};

TEST(PlayAudioModelTestCase, constructorSetsItself) {
	const auto device = std::make_shared<AudioDeviceStub>();
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
	PlayAudioModelTestCase,
	constructorThrowsDeviceFailureWhenDeviceFailsToInitialize)
{
	const auto device = std::make_shared<AudioDeviceStub>();
	device->fail();
	device->setErrorMessage("error.");
	assertDeviceFailureOnConstruction(device, "error.");
}

TEST(PlayAudioModelTestCase, playFirstClosesStreamThenOpensThenStarts) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto model = PlayAudioModelFacade::withValidParser(device);
	model->play({});
	assertEqual("close open start ", device->streamLog());
}

TEST(
	PlayAudioModelTestCase,
	playThrowsRequestErrorWhenDeviceFailure)
{
	try {
		const auto device = std::make_shared<AudioDeviceStub>();
		const auto model = PlayAudioModelFacade::withValidParser(device);
		device->fail();
		device->setErrorMessage("error.");
		model->play({});
		FAIL() << "Expected PlayAudioModel::RequestFailure";
	}
	catch (const PlayAudioModel::RequestFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(PlayAudioModelTestCase, playWhileStreamingDoesNotAlterCurrentStream) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto model = PlayAudioModelFacade::withValidParser(device);
	device->setStreaming();
	model->play({});
	EXPECT_TRUE(device->streamLog().empty());
}

TEST(PlayAudioModelTestCase, playPassesParametersToFactories) {
	const auto device = std::make_shared<AudioDeviceStub>();
	device->setDescriptions({ "a", "b", "c", "d", "e", "f", "g" });
	const auto reader = std::make_shared<AudioFileReaderStub>();
	reader->setSampleRate(48000);
	const auto parser = std::make_shared<FakeConfigurationFileParser>();
	parser->setValidSingleChannelDslProperties();
	parser->setValidBrirProperties();
	parser->setIntProperty(propertyName(binaural_room_impulse_response::Property::sampleRate), 48000);
	const auto compressorFactory = std::make_shared<MockCompressorFactory>();
	const auto audioFactory = std::make_shared<AudioFileReaderStubFactory>(reader);
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
	request.audioDevice = "e";
	request.level_dB_Spl = 1;
	request.attack_ms = 2;
	request.release_ms = 3;
	request.windowSize = 4;
	request.chunkSize = 5;
	model.play(request);
	EXPECT_EQ("c", audioFactory->filePath());
	EXPECT_EQ(2, compressorFactory->parameters().attack_ms);
	EXPECT_EQ(3, compressorFactory->parameters().release_ms);
	EXPECT_EQ(4, compressorFactory->parameters().windowSize);
	EXPECT_EQ(5, compressorFactory->parameters().chunkSize);
	EXPECT_EQ(5, device->streamParameters().framesPerBuffer);
	EXPECT_EQ(48000, device->streamParameters().sampleRate);
	EXPECT_EQ(2, device->streamParameters().channels);
	EXPECT_EQ(4, device->streamParameters().deviceIndex);
}

TEST(PlayAudioModelTestCase, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto model = PlayAudioModelFacade::withValidParser(device);
	model->play({});
	float left{};
	float right{};
	float *x[]{ &left, &right };
	device->fillStreamBuffer(x, 0);
	EXPECT_TRUE(device->setCallbackResultToCompleteCalled());
}

TEST(PlayAudioModelTestCase, playSetsCallbackResultToContinue) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto model = PlayAudioModelFacade::withValidParser(device);
	model->play({});
	EXPECT_TRUE(device->setCallbackResultToContinueCalled());
}

TEST(PlayAudioModelTestCase, audioDeviceDescriptionsReturnsDescriptions) {
	const auto device = std::make_shared<AudioDeviceStub>();
	device->setDescriptions({ "a", "b", "c" });
	PlayAudioModelFacade model{ device };
	assertEqual({ "a", "b", "c" }, model.audioDeviceDescriptions());
}

/*
class AudioFrameProcessorStub : public AudioFrameProcessor {
	int _frameCount{};
	float **_channels{};
public:
	int frameCount() const {
		return _frameCount;
	}

	const float *const *channels() const {
		return _channels;
	}

	void process(float ** channels, int frameCount) override {
		_channels = channels;
		_frameCount = frameCount;
	}
};

class ReadsAOne : public AudioFrameReader {
	void read(float ** channels, int) override {
		*channels[0] = 1;
	}
	bool complete() const override
	{
		return false;
	}
};

class AudioTimesTwo : public AudioFrameProcessor {
	void process(float ** channels, int) override {
		*channels[0] *= 2;
	}
};

TEST(PlayAudioModelTestCase, fillBufferReadsThenProcesses) {
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<AudioTimesTwo>();
	ProcessedAudioFrameReader stream{ reader, processor };
	float x{};
	float *channels[] = { &x };
	stream.read(channels, 0);
	EXPECT_EQ(2, x);
}

TEST(PlayAudioModelTestCase, fillBufferPassesParametersToReaderAndProcessor) {
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	const auto processor = std::make_shared<AudioFrameProcessorStub>();
	ProcessedAudioFrameReader stream{ reader, processor };
	float *x;
	stream.read(&x, 1);
	EXPECT_EQ(&x, reader->channels());
	EXPECT_EQ(1, reader->frameCount());
	EXPECT_EQ(&x, processor->channels());
	EXPECT_EQ(1, processor->frameCount());
}

TEST(PlayAudioModelTestCase, fillStreamBufferFillsFromStream) {
	const auto device = std::make_shared<AudioDeviceStub>();
	const auto stream = std::make_shared<AudioFrameReaderStub>();
	AudioDeviceController model{ device, stream };
	float *channel{};
	device->fillStreamBuffer(&channel, 1);
	EXPECT_EQ(&channel, stream->channels());
	EXPECT_EQ(1, stream->frameCount());
}
*/
