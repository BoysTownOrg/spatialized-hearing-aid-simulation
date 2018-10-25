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
	void fillStreamBuffer(void *x, int n) {
		_controller->fillStreamBuffer(x, n);
	}
};

class MockAudioStream : public AudioStream {
	int _frameCount{};
	float *_left{};
	float *_right{};
public:
	const float *left() const {
		return _left;
	}
	const float *right() const {
		return _right;
	}
	int frameCount() const {
		return _frameCount;
	}
	void fillBuffer(float *left, float *right, int frameCount) override {
		_left = left;
		_right = right;
		_frameCount = frameCount;
	}
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

TEST(AudioDeviceControllerTestCase, startAndStopStreaming) {
	const auto device = std::make_shared<MockAudioDevice>();
	AudioDeviceControllerFacade controller{ device };
	controller.startStreaming();
	EXPECT_TRUE(device->streaming());
	controller.stopStreaming();
	EXPECT_FALSE(device->streaming());
}

TEST(AudioDeviceControllerTestCase, fillStreamBufferFillsFromStream) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto stream = std::make_shared<MockAudioStream>();
	AudioDeviceController controller{ device, stream };
	float left;
	float right;
	float *x[] = { &left, &right };
	device->fillStreamBuffer(x, 1);
	EXPECT_EQ(&left, stream->left());
	EXPECT_EQ(&right, stream->right());
	EXPECT_EQ(1, stream->frameCount());
}