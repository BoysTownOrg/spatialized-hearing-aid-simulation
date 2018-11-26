#include "assert-utility.h"
#include <audio-device-control/AudioDeviceController.h>
#include <gtest/gtest.h>

class MockAudioDevice : public AudioDevice {
	std::string _errorMessage{};
	AudioDeviceController *_controller{};
	bool _streaming{};
	bool _failed{};
public:
	const AudioDeviceController *controller() const {
		return _controller;
	}
	void setController(AudioDeviceController *c) override {
		_controller = c;
	}
	bool streaming() const {
		return _streaming;
	}
	void startStream() override {
		_streaming = true;
	}
	void stopStream() override {
		_streaming = false;
	}
	void fillStreamBuffer(void *x, int n) {
		_controller->fillStreamBuffer(x, n);
	}
	void setFailedTrue() {
		_failed = true;
	}
	void setErrorMessage(std::string s) {
		_errorMessage = s;
	}
	bool failed() override {
		return _failed;
	}
	std::string errorMessage() override {
		return _errorMessage;
	}
};

class MockAudioFrameReader : public AudioFrameReader {
	int _frameCount{};
	float **_channels{};
public:
	const float * const * channels() const {
		return _channels;
	}
	int frameCount() const {
		return _frameCount;
	}
	void read(float **channels, int frameCount) override {
		_channels = channels;
		_frameCount = frameCount;
	}
};

class AudioDeviceControllerFacade {
	AudioDeviceController controller;
public:
	explicit AudioDeviceControllerFacade(
		std::shared_ptr<AudioDevice> device
	) :
		controller{ std::move(device), std::make_shared<MockAudioFrameReader>() } {}

	const AudioDeviceController *get() const {
		return &controller;
	}

	void startStreaming() {
		controller.startStreaming();
	}

	void stopStreaming() {
		controller.stopStreaming();
	}
};

class AudioDeviceControllerTestCase : public ::testing::TestCase {};

TEST(AudioDeviceControllerTestCase, constructorSetsItself) {
	const auto device = std::make_shared<MockAudioDevice>();
	AudioDeviceControllerFacade controller{ device };
	EXPECT_EQ(controller.get(), device->controller());
}

TEST(
	AudioDeviceControllerTestCase, 
	constructorThrowsDeviceConnectionFailureWhenDeviceError) 
{
	try {
		const auto device = std::make_shared<MockAudioDevice>();
		device->setFailedTrue();
		device->setErrorMessage("error.");
		AudioDeviceControllerFacade controller{ device };
		FAIL() << "Expected AudioDeviceController::DeviceConnectionFailure";
	}
	catch (const AudioDeviceController::DeviceConnectionFailure &e) {
		assertEqual("error.", e.what());
	}
}

TEST(AudioDeviceControllerTestCase, startAndStopStreaming) {
	const auto device = std::make_shared<MockAudioDevice>();
	AudioDeviceControllerFacade controller{ device };
	controller.startStreaming();
	EXPECT_TRUE(device->streaming());
	controller.stopStreaming();
	EXPECT_FALSE(device->streaming());
}

#include <functional>

static void assertFailedDeviceThrowsStreamingError(
	std::function<void(AudioDeviceControllerFacade &)> f) 
{
	try {
		const auto device = std::make_shared<MockAudioDevice>();
		AudioDeviceControllerFacade controller{ device };
		device->setFailedTrue();
		device->setErrorMessage("error.");
		f(controller);
		FAIL() << "Expected AudioDeviceController::StreamingError";
	}
	catch (const AudioDeviceController::StreamingError &e) {
		assertEqual("error.", e.what());
	}
}

TEST(
	AudioDeviceControllerTestCase,
	startAndStopThrowStreamingErrorWhenDeviceFailure)
{
	assertFailedDeviceThrowsStreamingError(
		[](AudioDeviceControllerFacade &c) { return c.startStreaming(); });
	assertFailedDeviceThrowsStreamingError(
		[](AudioDeviceControllerFacade &c) { return c.stopStreaming(); });
}

TEST(AudioDeviceControllerTestCase, fillStreamBufferFillsFromStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto stream = std::make_shared<MockAudioFrameReader>();
	AudioDeviceController controller{ device, stream };
	float *channel{};
	device->fillStreamBuffer(&channel, 1);
	EXPECT_EQ(&channel, stream->channels());
	EXPECT_EQ(1, stream->frameCount());
}
