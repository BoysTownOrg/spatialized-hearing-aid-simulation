#include "assert-utility.h"
#include "FakeConfigurationFileParser.h"
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <gtest/gtest.h>

class BinauralRoomImpulseResponseTester : public ::testing::TestCase {};

TEST(BinauralRoomImpulseResponseTester, returnsValuesParsed) {
	FakeConfigurationFileParser parser{};
	using namespace binaural_room_impulse_response;
	parser.setVectorProperty(propertyName(Property::leftImpulseResponse), { 1, 2, 3 });
	parser.setVectorProperty(propertyName(Property::rightImpulseResponse), { 4, 5, 6 });
	parser.setIntProperty(propertyName(Property::sampleRate), 7);
	BinauralRoomImpulseResponse response{ parser };
	assertEqual({ 1, 2, 3 }, response.left());
	assertEqual({ 4, 5, 6 }, response.right());
	EXPECT_EQ(7, response.sampleRate());
}

TEST(
	BinauralRoomImpulseResponseTester,
	throwsWhenParserThrows)
{
	try {
		BinauralRoomImpulseResponse{ ErrorParser{"error."} };
		FAIL() << "Expected BinauralRoomImpulseResponse::InvalidResponse.";
	}
	catch (const BinauralRoomImpulseResponse::InvalidResponse &e) {
		assertEqual("error.", e.what());
	}
}