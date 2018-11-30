#include "assert-utility.h"
#include "MockConfigurationFileParser.h"
#include <binaural-room-impulse-response-config/BrirConfig.h>
#include <gtest/gtest.h>

class BinauralRoomImpulseResponseTester : public ::testing::TestCase {};

TEST(BinauralRoomImpulseResponseTester, returnsValuesParsed) {
	MockConfigurationFileParser parser{};
	using brir_config::Property;
	parser.setVectorProperty(propertyName(Property::leftImpulseResponse), { 1, 2, 3 });
	parser.setVectorProperty(propertyName(Property::rightImpulseResponse), { 4, 5, 6 });
	parser.setIntProperty(propertyName(Property::sampleRate), 7);
	BinauralRoomImpulseResponse response{ parser };
	assertEqual({ 1, 2, 3 }, response.left());
	EXPECT_EQ(7, response.sampleRate());
}