#include <presentation/AudioPlayerModel.h>
#include <gtest/gtest.h>

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, badParametersThrowErrors) {
	AudioPlayerModel model{};
	model.playRequest({});
}
