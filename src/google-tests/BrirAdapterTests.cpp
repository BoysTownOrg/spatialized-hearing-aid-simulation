#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <gtest/gtest.h>

class BrirAdapterTests : public ::testing::Test {
protected:
	std::shared_ptr<FakeAudioFileReader> reader =
		std::make_shared<FakeAudioFileReader>();
	std::shared_ptr<FakeAudioFileReaderFactory> factory = 
		std::make_shared<FakeAudioFileReaderFactory>(reader);
	BrirAdapter adapter{ factory };

	BrirAdapter::BinauralRoomImpulseResponse read(std::string f = {}) {
		return adapter.read(std::move(f));
	}

	void assertReadThrowsReadError(std::string what) {
		try {
			read();
			FAIL() << "Expected BrirReader::ReadFailure";
		}
		catch (const BrirReader::ReadFailure &e) {
			assertEqual(what, e.what());
		}
	}
};

TEST_F(BrirAdapterTests, interpretsAudioFileAsBrir) {
	reader->setContents({ 1, 2, 3, 4 });
	reader->setChannels(2);
	reader->setSampleRate(5);
	const auto brir = read();
	EXPECT_EQ(5, brir.sampleRate);
	assertEqual({ 1, 3 }, brir.left);
	assertEqual({ 2, 4 }, brir.right);
}

TEST_F(BrirAdapterTests, singleChannelOnlyAppliesToLeftResponse) {
	reader->setContents({ 1, 2, 3, 4 });
	reader->setChannels(1);
	const auto brir = read();
	assertEqual({ 1, 2, 3, 4 }, brir.left);
	EXPECT_TRUE(brir.right.empty());
}

TEST_F(BrirAdapterTests, threeOrMoreChannelsIgnoresBeyondTheFirstTwo) {
	reader->setContents({ 1, 2, 3, 4, 5, 6 });
	reader->setChannels(3);
	const auto brir = read();
	assertEqual({ 1, 4 }, brir.left);
	assertEqual({ 2, 5 }, brir.right);
}

TEST_F(BrirAdapterTests, readPassesFilePathToFactory) {
	read("a");
	assertEqual("a", factory->filePath());
}

TEST_F(BrirAdapterTests, failedReaderThrowsReadError) {
	reader->fail();
	reader->setErrorMessage("error.");
	assertReadThrowsReadError("error.");
}