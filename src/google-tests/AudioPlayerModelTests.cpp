#include <playing-audio/AudioPlayerModel.h>
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

TEST(AudioPlayerModelTestCase, nonFloatsThrowRequestFailures) {
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
}

static void expectBadWindowSize(std::string size) {
	expectRequestTransformationYieldsFailure(
		[=](Model::PlayRequest request) {
			request.windowSize = size;
			return request;
		},
		"'" + size + "' is not a valid window size.");
}

static void expectBadChunkSize(std::string size) {
	expectRequestTransformationYieldsFailure(
		[=](Model::PlayRequest request) {
			request.chunkSize = size;
			return request;
		},
		"'" + size + "' is not a valid chunk size.");
}

TEST(AudioPlayerModelTestCase, nonPositiveIntegersThrowRequestFailures) {
	for (const auto s : std::vector<std::string>{ "a", "0.1", "-1" }) {
		expectBadWindowSize(s);
		expectBadChunkSize(s);
	}
}

TEST(AudioPlayerModelTestCase, parametersPassedToAudioPlayerFactory) {
	const auto factory = std::make_shared<MockAudioPlayerFactory>();
	AudioPlayerModel model{ factory };
	model.playRequest(
		{
			"a",
			"b",
			"c",
			"d",
			"1",
			"2",
			"3",
			"4",
			"5"
		});
	EXPECT_EQ("c", factory->audioFilePath());
}