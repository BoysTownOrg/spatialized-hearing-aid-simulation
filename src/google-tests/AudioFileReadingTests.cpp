#include <gtest/gtest.h>

class AudioFileReadingTestCase : public ::testing::TestCase {};

TEST(AudioFileReadingTestCase, tbd) {
	const auto reader = std::make_shared<MockAudioFileReader>({ 3, 4, 5, 6 });
	reader->setChannels(2);
	AudioFileInMemory audioFile{ reader };
	EXPECT_EQ({ 3 }, audioFile.readLeftChannel(1));
}