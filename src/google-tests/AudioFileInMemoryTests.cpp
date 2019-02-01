#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

namespace {
	class AudioFileInMemoryTests : public ::testing::Test {
	protected:
		FakeAudioFileReader reader{};
	};

	TEST_F(AudioFileInMemoryTests, emptyFileDoesNotThrowException) {
		reader.setContents({});
		AudioFileInMemory adapter{ reader };
	}

	class AudioFileInMemoryFacade {
		AudioFileInMemory inMemory;
	public:
		using buffer_type = std::vector<AudioFileInMemory::channel_type::element_type>;
		buffer_type left{};
		buffer_type right{};

		explicit AudioFileInMemoryFacade(AudioFileReader &reader) :
			inMemory{ reader } {}

		void readMonoFrames(buffer_type::size_type n) {
			left.resize(n);
			std::vector<AudioFileInMemory::channel_type> mono{ left };
			inMemory.read(mono);
		}

		void readStereoFrames(buffer_type::size_type n) {
			left.resize(n);
			right.resize(n);
			std::vector<AudioFileInMemory::channel_type> stereo{ left, right };
			inMemory.read(stereo);
		}

		auto complete() {
			return inMemory.complete();
		}

		auto remainingFrames() {
			return inMemory.remainingFrames();
		}

		auto reset() {
			return inMemory.reset();
		}
	};

	TEST_F(AudioFileInMemoryTests, readFillsChannel_Mono) {
		reader.setChannels(1);
		reader.setContents({ 1, 2, 3 });
		AudioFileInMemoryFacade facade{ reader };
		facade.readMonoFrames(3);
		assertEqual({ 1, 2, 3 }, facade.left);
	}

	TEST_F(AudioFileInMemoryTests, readFillsEachChannel_Stereo) {
		reader.setChannels(2);
		reader.setContents({ 1, 2, 3, 4, 5, 6 });
		AudioFileInMemoryFacade facade{ reader };
		facade.readStereoFrames(3);
		assertEqual({ 1, 3, 5 }, facade.left);
		assertEqual({ 2, 4, 6 }, facade.right);
	}

	TEST_F(AudioFileInMemoryTests, readNothingWhenExhausted) {
		reader.setContents({ 2 });
		AudioFileInMemoryFacade facade{ reader };
		facade.readMonoFrames(1);
		assertEqual({ 2 }, facade.left);
		facade.readMonoFrames(1);
		assertEqual({ 2 }, facade.left);
	}

	TEST_F(AudioFileInMemoryTests, completeWhenExhausted) {
		reader.setContents({ 2, 3 });
		AudioFileInMemoryFacade facade{ reader };
		facade.readMonoFrames(1);
		EXPECT_FALSE(facade.complete());
		facade.readMonoFrames(1);
		EXPECT_TRUE(facade.complete());
	}

	TEST_F(AudioFileInMemoryTests, completeWhenExhausted_ReadingMoreThanOneSampleAtATime) {
		reader.setContents({ 3, 4, 5, 6 });
		AudioFileInMemoryFacade facade{ reader };
		facade.readMonoFrames(2);
		EXPECT_FALSE(facade.complete());
		facade.readMonoFrames(2);
		EXPECT_TRUE(facade.complete());
	}

	TEST_F(AudioFileInMemoryTests, completeWhenExhausted_ReadingBeyondContents) {
		reader.setContents({ 2, 3, 4, 5 });
		AudioFileInMemoryFacade facade{ reader };
		facade.readMonoFrames(3);
		EXPECT_FALSE(facade.complete());
		facade.readMonoFrames(3);
		EXPECT_TRUE(facade.complete());
	}

	TEST_F(AudioFileInMemoryTests, returnsFramesRemaining) {
		reader.setContents({ 1, 2, 3 });
		AudioFileInMemoryFacade adapter{ reader };
		EXPECT_EQ(3, adapter.remainingFrames());
		adapter.readMonoFrames(1);
		EXPECT_EQ(2, adapter.remainingFrames());
		adapter.readMonoFrames(1);
		EXPECT_EQ(1, adapter.remainingFrames());
		adapter.readMonoFrames(1);
		EXPECT_EQ(0, adapter.remainingFrames());
	}

	TEST_F(AudioFileInMemoryTests, returnsFramesRemainingStereo) {
		reader.setContents({ 1, 2, 3, 4, 5, 6 });
		reader.setChannels(2);
		AudioFileInMemoryFacade adapter{ reader };
		EXPECT_EQ(3, adapter.remainingFrames());
		adapter.readStereoFrames(1);
		EXPECT_EQ(2, adapter.remainingFrames());
		adapter.readStereoFrames(1);
		EXPECT_EQ(1, adapter.remainingFrames());
		adapter.readStereoFrames(1);
		EXPECT_EQ(0, adapter.remainingFrames());
	}

	TEST_F(AudioFileInMemoryTests, returnsFileParameters) {
		reader.setContents({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });
		reader.setChannels(3);
		reader.setSampleRate(2);
		AudioFileInMemory adapter{ reader };
		EXPECT_EQ(12/3, adapter.frames());
		EXPECT_EQ(3, adapter.channels());
		EXPECT_EQ(2, adapter.sampleRate());
	}

	TEST_F(AudioFileInMemoryTests, seeksBeginningOnReset) {
		reader.setContents({ 3, 4 });
		AudioFileInMemoryFacade adapter{ reader };
		adapter.readMonoFrames(1);
		EXPECT_EQ(3, adapter.left.front());
		adapter.readMonoFrames(1);
		EXPECT_EQ(4, adapter.left.front());
		adapter.reset();
		adapter.readMonoFrames(1);
		EXPECT_EQ(3, adapter.left.front());
		adapter.readMonoFrames(1);
		EXPECT_EQ(4, adapter.left.front());
	}

	class AudioFileInMemoryFactoryTests : public ::testing::Test {
	protected:
		std::shared_ptr<FakeAudioFileReader> reader =
			std::make_shared<FakeAudioFileReader>();
		FakeAudioFileReaderFactory factory{ reader };
		AudioFileInMemoryFactory adapter{ &factory };

		void assertMakeThrowsCreateError(std::string what) {
			try {
				make();
				FAIL() << "Expected AudioFrameReaderFactory::CreateError";
			}
			catch (const AudioFrameReaderFactory::CreateError &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void make(std::string f = {}) {
			adapter.make(std::move(f));
		}
	};

	TEST_F(AudioFileInMemoryFactoryTests, factoryThrowsCreateErrorOnFileError) {
		reader->fail();
		reader->setErrorMessage("error.");
		assertMakeThrowsCreateError("error.");
	}

	TEST_F(AudioFileInMemoryFactoryTests, factoryPassesFilePath) {
		make("a");
		assertEqual("a", factory.filePath());
	}
}