#include <gtest/gtest.h>

class BrirAdapterTestCase : public ::testing::TestCase {};

TEST(BrirAdapterTestCase, tbd) {
	const auto reader = std::make_shared<FakeAudioFileReader>({ 1, 2, 3, 4 });
	reader->setChannels(2);
	reader->setSampleRate(5);
	BrirAdapter adapter{ *reader };
	EXPECT_EQ(5, adapter.asInt(propertyName(binaural_room_impulse_response::Property::sampleRate)));
}