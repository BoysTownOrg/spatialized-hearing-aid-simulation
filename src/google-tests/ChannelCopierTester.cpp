#include "AudioFrameReaderStub.h"
#include "FakeAudioFile.h"
#include "assert-utility.h"
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <audio-file-reading-writing/ChannelCopier.h>
#include <gtest/gtest.h>

namespace {
	class ChannelCopierTests : public ::testing::Test {
	protected:
		std::shared_ptr<AudioFrameReaderStub> decorated =
			std::make_shared<AudioFrameReaderStub>();
		ChannelCopier copier{ decorated };
	};

	TEST_F(ChannelCopierTests, completeWhenDecoratedReaderIsComplete) {
		assertFalse(copier.complete());
		decorated->setComplete();
		assertTrue(copier.complete());
	}

	TEST_F(ChannelCopierTests, returnsDecoratedReaderParameters) {
		decorated->setSampleRate(1);
		decorated->setFrames(2);
		decorated->setChannels(3);
		decorated->setRemainingFrames(4);
		assertEqual(1, copier.sampleRate());
		assertEqual(2LL, copier.frames());
		assertEqual(3, copier.channels());
		assertEqual(4LL, copier.remainingFrames());
	}

	TEST_F(ChannelCopierTests, returnsTwoIfDecoratedReaderHasOnlyOneChannel) {
		decorated->setChannels(1);
		assertEqual(2, copier.channels());
	}

	TEST_F(ChannelCopierTests, resetsDecoratedReaderWhenReset) {
		copier.reset();
		assertTrue(decorated->log().contains("reset "));
	}

	class ChannelCopierFacade {
		ChannelCopier copier;
	public:
		using channel_type = ChannelCopier::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;
		buffer_type left{};
		buffer_type right{};

		explicit ChannelCopierFacade(std::shared_ptr<AudioFrameReader> r) noexcept : 
			copier{ std::move(r) } {}

		void readStereoFrames(buffer_type::size_type n) {
			left.resize(n);
			right.resize(n);
			std::vector<channel_type> stereo{ left, right };
			copier.read(stereo);
		}

		void readEmptyAudio() {
			std::vector<channel_type> empty{};
			copier.read(empty);
		}
	};

	class ChannelCopierDecorateTests : public ::testing::Test {
	protected:
		ChannelCopierFacade copyInMemoryReader(AudioFileReader &r) {
			return ChannelCopierFacade{ std::make_shared<AudioFileInMemory>(r) };
		}
	};

	TEST_F(
		ChannelCopierDecorateTests, 
		copiesFirstChannelToSecondWhenDecoratedReaderHasOnlyOneChannel
	) {
		FakeAudioFileReader reader;
		reader.setContents({ 1, 2, 3 });
		reader.setChannels(1);
		auto copier = copyInMemoryReader(reader);
		copier.readStereoFrames(3);
		assertEqual({ 1, 2, 3 }, copier.left);
		assertEqual({ 1, 2, 3 }, copier.right);
	}

	TEST_F(
		ChannelCopierDecorateTests, 
		ableToReadEmptyAudioWithoutThrowing
	) {
		FakeAudioFileReader reader;
		auto copier = copyInMemoryReader(reader);
		copier.readEmptyAudio();
	}

	class ChannelCopierFactoryTests : public ::testing::Test {
	protected:
		AudioFrameReaderStubFactory factory;
		ChannelCopierFactory adapter{ &factory };
	};

	TEST_F(ChannelCopierFactoryTests, factoryPassesFilePath) {
		adapter.make("a");
		assertEqual("a", factory.filePath());
	}
}
