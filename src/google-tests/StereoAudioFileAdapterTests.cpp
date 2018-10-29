#include <gtest/gtest.h>

class StereoAudioFileAdapterTestCase : public ::testing::TestCase {};

TEST(StereoAudioFileAdapterTestCase, tbd) {
	const auto file = std::make_shared<MockStereoAudioFile>();
	StereoAudioFileAdapter adapter{ file };
	float left{};
	float right{};
	float *channels[] = { &left, &right };
	adapter.read(channels, 1);
	EXPECT_EQ(&left, file->left());
	EXPECT_EQ(&right, file->right());
	EXPECT_EQ(1, file->frameCount());
}