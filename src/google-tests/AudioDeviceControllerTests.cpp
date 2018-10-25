#include <common-includes/Interface.h>

class AudioDeviceController;

class AudioDevice {
public:
	INTERFACE_OPERATIONS(AudioDevice);
	virtual void setController(AudioDeviceController *) = 0;
};

class AudioStream {};

#include <memory>

class AudioDeviceController {
public:
	AudioDeviceController(
		std::shared_ptr<AudioDevice> device,
		std::shared_ptr<AudioStream>
	) 
	{
		device->setController(this);
	}
};

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