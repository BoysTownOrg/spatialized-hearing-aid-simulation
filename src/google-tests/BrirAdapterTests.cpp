#include <audio-file-reading/AudioFileReader.h>
#include <dsl-prescription/ConfigurationFileParser.h>
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <gsl/gsl>

class BrirAdapter : public ConfigurationFileParser {
	using vector_type = std::vector<double>;
	using size_type = vector_type::size_type;
	std::vector<double> left{};
	std::vector<double> right{};
	int sampleRate;
public:
	explicit BrirAdapter(AudioFileReader &reader) :
		sampleRate(reader.sampleRate()) 
	{
		std::vector<double> buffer(gsl::narrow<size_type>(reader.frames() * reader.channels()));
		reader.readFrames(&buffer[0], buffer.size());
		bool oddSample = false;
		std::partition_copy(
			buffer.begin(),
			buffer.end(),
			std::back_inserter(left),
			std::back_inserter(right),
			[&oddSample](double) { return oddSample = !oddSample; });
	}

	std::vector<double> asVector(std::string property) const override {
		if (property == propertyName(binaural_room_impulse_response::Property::leftImpulseResponse))
			return left;
		else
			return right;
	}

	double asDouble(std::string ) const override {
		return 0.0;
	}

	int asInt(std::string) const override {
		return sampleRate;
	}
};

#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <gtest/gtest.h>

class BrirAdapterTestCase : public ::testing::TestCase {};

TEST(BrirAdapterTestCase, interpretsAudioFileAsBrir) {
	FakeAudioFileReader reader{ { 1, 2, 3, 4 } };
	reader.setChannels(2);
	reader.setSampleRate(5);
	BrirAdapter adapter{ reader };
	EXPECT_EQ(5, adapter.asInt(propertyName(binaural_room_impulse_response::Property::sampleRate)));
	assertEqual({ 1, 3 }, adapter.asVector(propertyName(binaural_room_impulse_response::Property::leftImpulseResponse)));
	assertEqual({ 2, 4 }, adapter.asVector(propertyName(binaural_room_impulse_response::Property::rightImpulseResponse)));
}