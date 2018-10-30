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

static void expectRequestTransformationYieldsFailure(
	std::function<Model::PlayRequest(Model::PlayRequest)> transformation,
	std::string message) 
{
	try {
		AudioPlayerModel model{};
		model.playRequest(transformation(validRequest()));
		FAIL() << "Expected Model::RequestFailure";
	}
	catch (const Model::RequestFailure &failure) {
		expectEqual(message, failure.what());
	}
}

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, badParametersThrowRequestFailures) {
	expectRequestTransformationYieldsFailure(
		[](Model::PlayRequest request) { 
			request.level_dB_Spl = "a";
			return request;
		},
		"'a' is not a valid level.");
	expectRequestTransformationYieldsFailure(
		[](Model::PlayRequest request) {
			request.attack_ms = "a";
			return request;
		},
		"'a' is not a valid attack time.");
	expectRequestTransformationYieldsFailure(
		[](Model::PlayRequest request) {
			request.release_ms = "a";
			return request;
		},
		"'a' is not a valid release time.");
	expectRequestTransformationYieldsFailure(
		[](Model::PlayRequest request) {
			request.windowSize = "a";
			return request;
		},
		"'a' is not a valid window size.");
	expectRequestTransformationYieldsFailure(
		[](Model::PlayRequest request) {
			request.windowSize = "0.1";
			return request;
		},
		"'0.1' is not a valid window size.");
	expectRequestTransformationYieldsFailure(
		[](Model::PlayRequest request) {
			request.chunkSize = "a";
			return request;
		},
		"'a' is not a valid chunk size.");
	expectRequestTransformationYieldsFailure(
		[](Model::PlayRequest request) {
			request.chunkSize = "0.1";
			return request;
		},
		"'0.1' is not a valid chunk size.");
}
