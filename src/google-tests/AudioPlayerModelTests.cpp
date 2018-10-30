#include <playing-audio/AudioPlayerModel.h>
#include <gtest/gtest.h>
#include <functional>

class MockAudioPlayer : public AudioPlayer {
	bool _played{};
public:
	bool played() const {
		return _played;
	}
	void play() override {
		_played = true;
	}
};

class MockAudioPlayerFactory : public AudioPlayerFactory {
	std::string _leftDslPrescriptionFilePath{};
	std::string _rightDslPrescriptionFilePath{};
	std::string _audioFilePath{};
	std::string _brirFilePath{};
	double _level_dB_Spl{};
	double _attack_ms{};
	double _release_ms{};
	int _windowSize{};
	int _chunkSize{};
	int _framesPerBuffer{};
	int _audioDeviceSampleRate{};
	int _hearingAidSampleRate{};
	std::shared_ptr<AudioPlayer> player;
public:
	explicit MockAudioPlayerFactory(
		std::shared_ptr<AudioPlayer> player =
			std::make_shared<MockAudioPlayer>()
	) :
		player{ std::move(player) } {}

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
	double level_dB_Spl() const {
		return _level_dB_Spl;
	}
	double attack_ms() const {
		return _attack_ms;
	}
	double release_ms() const {
		return _release_ms;
	}
	int windowSize() const {
		return _windowSize;
	}
	int chunkSize() const {
		return _chunkSize;
	}
	int framesPerBuffer() const {
		return _framesPerBuffer;
	}
	int audioDeviceSampleRate() const {
		return _audioDeviceSampleRate;
	}
	int hearingAidSampleRate() const {
		return _hearingAidSampleRate;
	}
	std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters p) override
	{
		_audioFilePath = p.audioFilePath;
		_brirFilePath = p.forSpatialization.brirFilePath;
		_leftDslPrescriptionFilePath = p.forHearingAidSimulation.leftDslPrescriptionFilePath;
		_rightDslPrescriptionFilePath = p.forHearingAidSimulation.rightDslPrescriptionFilePath;
		_level_dB_Spl = p.forHearingAidSimulation.level_dB_Spl;
		_attack_ms = p.forHearingAidSimulation.attack_ms;
		_release_ms = p.forHearingAidSimulation.release_ms;
		_windowSize = p.forHearingAidSimulation.windowSize;
		_chunkSize = p.forHearingAidSimulation.chunkSize;
		_framesPerBuffer = p.forAudioDevice.framesPerBuffer;
		_audioDeviceSampleRate = p.forAudioDevice.sampleRate;
		_hearingAidSampleRate = p.forHearingAidSimulation.sampleRate;
		return player;
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
	EXPECT_EQ(5, factory->framesPerBuffer());
	EXPECT_EQ(44100, factory->hearingAidSampleRate());
	EXPECT_EQ(44100, factory->audioDeviceSampleRate());
}

TEST(AudioPlayerModelTestCase, successfulRequestPlaysPlayer) {
	const auto player = std::make_shared<MockAudioPlayer>();
	const auto factory = std::make_shared<MockAudioPlayerFactory>(player);
	AudioPlayerModel model{ factory };
	model.playRequest(validRequest());
	EXPECT_TRUE(player->played());
}
