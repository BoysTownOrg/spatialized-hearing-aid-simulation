#include <binaural-room-impulse-response-config/BrirConfig.h>
#include <dsl-prescription/ConfigurationFileParser.h>

class BinauralRoomImpulseResponse {
	int _sampleRate;
public:
	explicit BinauralRoomImpulseResponse(
		const ConfigurationFileParser &parser
	) :
		_sampleRate{ parser.asInt(propertyName(brir_config::Property::sampleRate)) } {}

	int sampleRate() const {
		return _sampleRate;
	}
};

#include "MockConfigurationFileParser.h"
#include <gtest/gtest.h>

class BinauralRoomImpulseResponseTester : public ::testing::TestCase {};

TEST(BinauralRoomImpulseResponseTester, tbd) {
	MockConfigurationFileParser parser{};
	using brir_config::Property;
	parser.setVectorProperty(propertyName(Property::leftImpulseResponse), { 1, 2, 3 });
	parser.setVectorProperty(propertyName(Property::rightImpulseResponse), { 4, 5, 6 });
	parser.setIntProperty(propertyName(Property::sampleRate), 7);
	BinauralRoomImpulseResponse response{ parser };
	EXPECT_EQ(7, response.sampleRate());
}