#include "assert-utility.h"
#include "MockAudioDevice.h"
#include "MockAudioFrameReader.h"
#include <audio-device-control/AudioDeviceController.h>
#include <gtest/gtest.h>

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

	void openStream() {
		controller.openStream();
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

TEST(AudioDeviceControllerTestCase, openNewStreamFirstClosesStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	AudioDeviceControllerFacade controller{ device };
	controller.openStream();
	assertEqual("close open ", device->streamLog());
}

TEST(AudioDeviceControllerTestCase, openStreamPassesStreamParameters) {
	const auto device = std::make_shared<MockAudioDevice>();
	AudioDeviceControllerFacade controller{ device };
	AudioDevice::Parameters p;
	p.channels = { 1 };
	p.framesPerBuffer = 2;
	p.sampleRate = 3;
	controller.openStream(p);
	assertEqual({ 1 }, device->streamParameters().channels);
	EXPECT_EQ(2, device->streamParameters().framesPerBuffer);
	EXPECT_EQ(3, device->streamParameters().sampleRate);
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
		[](AudioDeviceControllerFacade &c) { return c.openStream(); });
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

TEST(AudioDeviceControllerTestCase, fillStreamBufferSetsCallbackResultToCompleteWhenComplete) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto reader = std::make_shared<MockAudioFrameReader>();
	AudioDeviceController controller{ device, reader };
	reader->setComplete();
	device->fillStreamBuffer(nullptr, 0);
	EXPECT_TRUE(device->setCallbackResultToCompleteCalled());
}
