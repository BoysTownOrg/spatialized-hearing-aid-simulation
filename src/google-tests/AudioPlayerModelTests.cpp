#include <presentation/AudioPlayerModel.h>
#include <gtest/gtest.h>
#include <functional>

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

static void expectRequestAlterationYieldsFailure(
	std::function<void(Model::PlayRequest &)> alteration,
	std::string message) 
{
	try {
		AudioPlayerModel model{};
		auto request = validRequest();
		alteration(request);
		model.playRequest(request);
		FAIL() << "Expected Model::RequestFailure";
	}
	catch (const Model::RequestFailure &failure) {
		expectEqual(message, failure.what());
	}
}

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, badParametersThrowRequestFailures) {
	expectRequestAlterationYieldsFailure(
		[](Model::PlayRequest &request) { request.level_dB_Spl = "a"; },
		"'a' is not a valid level.");
	expectRequestAlterationYieldsFailure(
		[](Model::PlayRequest &request) { request.attack_ms = "a"; },
		"'a' is not a valid attack time.");
}
