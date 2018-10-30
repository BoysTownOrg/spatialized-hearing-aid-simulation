#include <presentation/AudioPlayerModel.h>
#include <gtest/gtest.h>

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, badParametersReturnErrorMessages) {
	AudioPlayerModel model{};
	const auto response = model.playRequest(
		{
			"",
			"",
			"",
			"",
			"a",
			"2.0",
			"3.0",
			"4",
			"5"
		});
	EXPECT_EQ(Model::Status::fail, response.status);
}
