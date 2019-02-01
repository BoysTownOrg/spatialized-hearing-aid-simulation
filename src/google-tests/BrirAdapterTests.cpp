#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <gtest/gtest.h>

namespace {
	class BrirAdapterTests : public ::testing::Test {
	protected:
		std::shared_ptr<FakeAudioFileReader> reader =
			std::make_shared<FakeAudioFileReader>();
		FakeAudioFileReaderFactory factory{ reader };
		BrirAdapter adapter{ &factory };

		auto read(std::string f = {}) {
			return adapter.read(std::move(f));
		}

		void assertReadThrowsReadError(std::string what) {
			try {
				read();
				FAIL() << "Expected BrirReader::ReadFailure";
			}
			catch (const BrirReader::ReadFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
	};

	TEST_F(BrirAdapterTests, readsFirstAudioChannelIntoLeftResponse) {
		reader->setChannels(2);
		reader->setContents({ 1, 2, 3, 4 });
		const auto brir = read();
		assertEqual({ 1, 3 }, brir.left);
	}

	TEST_F(BrirAdapterTests, readsSecondAudioChannelIntoRightResponse) {
		reader->setChannels(2);
		reader->setContents({ 1, 2, 3, 4 });
		const auto brir = read();
		assertEqual({ 2, 4 }, brir.right);
	}

	TEST_F(BrirAdapterTests, brirSampleRateIsAudioFileSampleRate) {
		reader->setSampleRate(1);
		const auto brir = read();
		EXPECT_EQ(1, brir.sampleRate);
	}

	TEST_F(BrirAdapterTests, singleChannelOnlyAppliesToLeftResponse) {
		reader->setChannels(1);
		reader->setContents({ 1, 2, 3, 4 });
		const auto brir = read();
		assertEqual({ 1, 2, 3, 4 }, brir.left);
		EXPECT_TRUE(brir.right.empty());
	}

	TEST_F(BrirAdapterTests, threeOrMoreChannelsIgnoresBeyondTheFirstTwo) {
		reader->setChannels(3);
		reader->setContents({ 1, 2, 3, 4, 5, 6 });
		const auto brir = read();
		assertEqual({ 1, 4 }, brir.left);
		assertEqual({ 2, 5 }, brir.right);
	}

	TEST_F(BrirAdapterTests, readPassesFilePathToFactory) {
		read("a");
		assertEqual("a", factory.filePath());
	}

	TEST_F(BrirAdapterTests, failedReaderThrowsReadError) {
		reader->fail();
		reader->setErrorMessage("error.");
		assertReadThrowsReadError("error.");
	}
}