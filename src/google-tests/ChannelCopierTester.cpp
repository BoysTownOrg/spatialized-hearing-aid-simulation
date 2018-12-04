#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFileReaderStub.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <audio-stream-processing/ChannelCopier.h>
#include <gtest/gtest.h>

class ChannelCopierTestCase : public ::testing::TestCase {};

TEST(ChannelCopierTestCase, copiesFirstChannelToSecond) {
	AudioFileReaderStub reader{ std::vector<float>{ 1, 2, 3 } };
	ChannelCopier copier{ std::make_shared<AudioFileInMemory>(reader) };
	std::vector<float> left(3);
	std::vector<float> right(3);
	float *x[]{ &left[0], &right[0] };
	copier.read(x, 3);
	assertEqual({ 1, 2, 3 }, left);
	assertEqual({ 1, 2, 3 }, right);
}

TEST(ChannelCopierTestCase, returnsCompleteWhenComplete) {
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	ChannelCopier copier{ reader };
	EXPECT_FALSE(copier.complete());
	reader->setComplete();
	EXPECT_TRUE(copier.complete());
}