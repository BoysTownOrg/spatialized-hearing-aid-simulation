#include <presentation/AudioPlayerModel.h>
#include <gtest/gtest.h>

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, badLevelThrowsRequestFailure) {
	try {
		AudioPlayerModel model{};
		model.playRequest(
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
		FAIL() << "Expected Model::RequestFailure";
	}
	catch (const Model::RequestFailure &failure) {
		EXPECT_EQ(std::string{ "'a' is not a valid level..." }, failure.what());
	}
}
