#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <gtest/gtest.h>

class BrirAdapterTestCase : public ::testing::TestCase {};

TEST(BrirAdapterTestCase, interpretsAudioFileAsBrir) {
	const auto reader = std::make_shared<FakeAudioFileReader>( std::vector<float>{ 1, 2, 3, 4 } );
	reader->setChannels(2);
	reader->setSampleRate(5);
	BrirAdapter adapter{ std::make_shared<FakeAudioFileReaderFactory>(reader) };
	const auto brir = adapter.read("");
	EXPECT_EQ(5, brir.sampleRate);
	assertEqual({ 1, 3 }, brir.left);
	assertEqual({ 2, 4 }, brir.right);
}

TEST(BrirAdapterTestCase, singleChannelOnlyAppliesToLeftResponse) {
	const auto reader = std::make_shared<FakeAudioFileReader>(std::vector<float>{ 1, 2, 3, 4 });
	reader->setChannels(1);
	BrirAdapter adapter{ std::make_shared<FakeAudioFileReaderFactory>(reader) };
	const auto brir = adapter.read("");
	assertEqual({ 1, 2, 3, 4 }, brir.left);
	EXPECT_TRUE(brir.right.empty());
}

TEST(BrirAdapterTestCase, threeOrMoreChannelsIgnoresBeyondTheFirstTwo) {
	const auto reader = std::make_shared<FakeAudioFileReader>(std::vector<float>{ 1, 2, 3, 4, 5, 6 });
	reader->setChannels(3);
	BrirAdapter adapter{ std::make_shared<FakeAudioFileReaderFactory>(reader) };
	const auto brir = adapter.read("");
	assertEqual({ 1, 4 }, brir.left);
	assertEqual({ 2, 5 }, brir.right);
}

TEST(BrirAdapterTestCase, failedReaderThrowsReadError) {
	try {
		const auto reader = std::make_shared<FakeAudioFileReader>();
		reader->fail();
		reader->setErrorMessage("error.");
		BrirAdapter adapter{ std::make_shared<FakeAudioFileReaderFactory>(reader) };
		adapter.read("");
		FAIL() << "Expected BrirReader::ReadError";
	}
	catch (const BrirReader::ReadError &e) {
		assertEqual("error.", e.what());
	}
}