#include <audio-device-control/AudioDeviceController.h>
#include <gtest/gtest.h>

class MockAudioDevice : public AudioDevice {
	AudioDeviceController *_controller{};
public:
	const AudioDeviceController *controller() const {
		return _controller;
	}
	void setController(AudioDeviceController *c) override {
		_controller = c;
	}
};

class MockAudioStream : public AudioStream {

};

class AudioDeviceControllerTestCase : public ::testing::TestCase {};

TEST(AudioDeviceControllerTestCase, constructorSetsItself) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto stream = std::make_shared<MockAudioStream>();
	AudioDeviceController controller{ device, stream };
	EXPECT_EQ(&controller, device->controller());
}

TEST(AudioDeviceControllerTestCase, startStreamingStartsStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto stream = std::make_shared<MockAudioStream>();
	AudioDeviceController controller{ device, stream };
	controller.startStreaming();
	EXPECT_TRUE(stream->streaming());
}