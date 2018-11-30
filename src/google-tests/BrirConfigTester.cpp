#include <dsl-prescription/ConfigurationFileParser.h>

class BinauralRoomImpulseResponse {
public:
	explicit BinauralRoomImpulseResponse(
		const ConfigurationFileParser &
	) {}

	int sampleRate() const {
		return 0;
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