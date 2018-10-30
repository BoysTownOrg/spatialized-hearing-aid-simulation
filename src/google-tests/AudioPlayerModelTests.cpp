#include <presentation/AudioPlayerModel.h>
#include <gtest/gtest.h>

static Model::PlayRequest validRequest() {
	return 
		{
			"",
			"",
			"",
			"",
			"0.0",
			"0.0",
			"0.0",
			"0",
			"0"
		};
}

static void expectEqual(std::string expected, std::string actual) {
	EXPECT_EQ(expected, actual);
}

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, badLevelThrowsRequestFailure) {
	try {
		AudioPlayerModel model{};
		auto request = validRequest();
		request.level_dB_Spl = "a";
		model.playRequest(request);
		FAIL() << "Expected Model::RequestFailure";
	}
	catch (const Model::RequestFailure &failure) {
		expectEqual("'a' is not a valid level.", failure.what());
	}
}

TEST(AudioPlayerModelTestCase, badAttackThrowsRequestFailure) {
	try {
		AudioPlayerModel model{};
		auto request = validRequest();
		request.attack_ms = "a";
		model.playRequest(request);
		FAIL() << "Expected Model::RequestFailure";
	}
	catch (const Model::RequestFailure &failure) {
		expectEqual("'a' is not a valid attack time.", failure.what());
	}
}
