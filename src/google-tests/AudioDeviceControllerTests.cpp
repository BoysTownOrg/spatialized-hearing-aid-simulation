#include <gtest/gtest.h>

class AudioDeviceControllerTestCase : public ::testing::TestCase {};

TEST(AudioDeviceControllerTestCase, constructorSetsItself) {
	const auto device = std::make_shared<MockAudioDevice>();
	const auto stream = std::make_shared<MockAudioStream>();
	AudioDeviceController controller{ device, stream };
	EXPECT_EQ(&controller, device->controller());
}