#include "assert-utility.h"
#include "MockAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <audio-stream-processing/ChannelCopier.h>
#include <gtest/gtest.h>

class ChannelCopierTestCase : public ::testing::TestCase {};

TEST(ChannelCopierTestCase, copiesFirstChannelToSecond) {
	const auto reader = std::make_shared<MockAudioFileReader>(std::vector<float>{ 1, 2, 3 });
	reader->setChannels(1);
	const auto adapter = std::make_shared<AudioFileInMemory>(*reader);
	ChannelCopier copier{ adapter };
	std::vector<float> left(3);
	std::vector<float> right(3);
	float *x[]{ &left[0], &right[0] };
	copier.read(x, 3);
	assertEqual({ 1, 2, 3 }, left);
	assertEqual({ 1, 2, 3 }, right);
}