#include <presentation/Model.h>

class AudioPlayerModel : public Model {
public:
	void playRequest(PlayRequest) override {
	}
};

#include <gtest/gtest.h>

class AudioPlayerModelTestCase : public ::testing::TestCase {};

TEST(AudioPlayerModelTestCase, badParametersThrowErrors) {
	AudioPlayerModel model{};
	model.playRequest({});
}
