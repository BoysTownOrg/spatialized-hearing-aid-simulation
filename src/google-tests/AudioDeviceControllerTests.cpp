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

class MockAudioFrameReader : public AudioFrameReader {
	int _frameCount{};
	float **_frames{};
public:
	const float * const * channels() const {
		return _frames;
	}
	int frameCount() const {
		return _frameCount;
	}
	void read(float **channels, int frameCount) override {
		_frames = channels;
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
	const auto stream = std::make_shared<MockAudioFrameReader>();
	AudioDeviceController controller{ device, stream };
	float *frame{};
	device->fillStreamBuffer(&frame, 1);
	EXPECT_EQ(&frame, stream->channels());
	EXPECT_EQ(1, stream->frameCount());
}
