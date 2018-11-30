#include <gtest/gtest.h>

class ChannelCopierTestCase : public ::testing::TestCase {};

TEST(ChannelCopierTestCase, tbd) {
	MockAudioFileReader reader{ { 1, 2, 3 } };
	reader->setChannels(1);
	AudioFileInMemory adapter{ reader };
	ChannelCopier copier{ adapter };
	std::vector<float> left(3);
	std::vector<float> right(3);
	float *x[]{ &left[0], &right[0] };
	copier.read(x, 3);
	assertEqual({ 1, 2, 3 }, left);
	assertEqual({ 1, 2, 3 }, right);
}