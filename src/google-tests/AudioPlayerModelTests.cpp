#include <playing-audio/AudioPlayerModel.h>
#include <gtest/gtest.h>
#include <functional>

class MockAudioPlayerFactory : public AudioPlayerFactory {
	std::string _leftDslPrescriptionFilePath{};
	std::string _rightDslPrescriptionFilePath{};
	std::string _audioFilePath{};
	std::string _brirFilePath{};
public:
	std::string audioFilePath() const {
		return _audioFilePath;
	}
	std::string leftDslPrescriptionFilePath() const {
		return _leftDslPrescriptionFilePath;
	}
	std::string rightDslPrescriptionFilePath() const {
		return _rightDslPrescriptionFilePath;
	}
	std::string brirFilePath() const {
		return _brirFilePath;
	}
	std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters p) override
	{
		_audioFilePath = p.audioFilePath;
		_leftDslPrescriptionFilePath = p.leftDslPrescriptionFilePath;
		_rightDslPrescriptionFilePath = p.rightDslPrescriptionFilePath;
		_brirFilePath = p.brirFilePath;
		return {};
	}
};

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
		AudioPlayerModel model{std::make_shared<MockAudioPlayerFactory>()};
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
	EXPECT_EQ("a", factory->leftDslPrescriptionFilePath());
	EXPECT_EQ("b", factory->rightDslPrescriptionFilePath());
	EXPECT_EQ("c", factory->audioFilePath());
	EXPECT_EQ("d", factory->brirFilePath());
	EXPECT_EQ(1, factory->level_dB_Spl());
	EXPECT_EQ(2, factory->attack_ms());
	EXPECT_EQ(3, factory->release_ms());
	EXPECT_EQ(4, factory->windowSize());
	EXPECT_EQ(5, factory->chunkSize());
}
