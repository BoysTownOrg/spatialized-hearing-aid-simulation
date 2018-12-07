#include <audio-file-reading/AudioFileReader.h>
#include <dsl-prescription/ConfigurationFileParser.h>

class BrirAdapter : public ConfigurationFileParser {
public:
	explicit BrirAdapter(AudioFileReader &) {}

	std::vector<double> asVector(std::string ) const override {
		return std::vector<double>();
	}

	double asDouble(std::string ) const override {
		return 0.0;
	}

	int asInt(std::string ) const override {
		return 0;
	}
};

#include "FakeAudioFileReader.h"
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <gtest/gtest.h>

class BrirAdapterTestCase : public ::testing::TestCase {};

TEST(BrirAdapterTestCase, tbd) {
	FakeAudioFileReader reader{ { 1, 2, 3, 4 } };
	reader.setChannels(2);
	reader.setSampleRate(5);
	BrirAdapter adapter{ reader };
	EXPECT_EQ(5, adapter.asInt(propertyName(binaural_room_impulse_response::Property::sampleRate)));
}