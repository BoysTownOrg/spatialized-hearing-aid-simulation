#include <audio-file-reading/StereoAudioFileAdapter.h>
#include <gtest/gtest.h>

class MockStereoAudioFile : public StereoAudioFile {
	float *_left{};
	float *_right{};
	int _samples{};
public:
	const float *left() const {
		return _left;
	}
	const float *right() const {
		return _right;
	}
	int samples() const {
		return _samples;
	}
	void read(float * left, float * right, int samples) override {
		_left = left;
		_right = right;
		_samples = samples;
	}
};

class StereoAudioFileAdapterTestCase : public ::testing::TestCase {};

TEST(StereoAudioFileAdapterTestCase, readPassesParameters) {
	const auto file = std::make_shared<MockStereoAudioFile>();
	StereoAudioFileAdapter adapter{ file };
	float left{};
	float right{};
	float *channels[] = { &left, &right };
	adapter.read(channels, 1);
	EXPECT_EQ(&left, file->left());
	EXPECT_EQ(&right, file->right());
	EXPECT_EQ(1, file->samples());
}
