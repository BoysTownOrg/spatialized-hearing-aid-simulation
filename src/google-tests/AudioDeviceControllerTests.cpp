#include <audio-device-control/AudioDeviceController.h>
#include <gtest/gtest.h>

class MockAudioDevice : public AudioDevice {
	AudioDeviceController *_controller{};
	bool _streaming{};
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
};

class MockAudioStream : public AudioStream {
};

class AudioDeviceControllerFacade {
	AudioDeviceController controller;
public:
	explicit AudioDeviceControllerFacade(
		std::shared_ptr<AudioDevice> device
	) :
		controller{ std::move(device), std::make_shared<MockAudioStream>() } {}

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

TEST(AudioDeviceControllerTestCase, startStreamingStartsStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	AudioDeviceControllerFacade controller{ device };
	controller.startStreaming();
	EXPECT_TRUE(device->streaming());
}

TEST(AudioDeviceControllerTestCase, stopStreamingStopsStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	AudioDeviceControllerFacade controller{ device };
	controller.startStreaming();
	controller.stopStreaming();
	EXPECT_FALSE(device->streaming());
}