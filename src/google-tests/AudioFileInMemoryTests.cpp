#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

namespace {
	class AudioFileInMemoryFacade {
		AudioFileInMemory inMemory;
	public:
		using vector_type = std::vector<float>;
		vector_type left{};
		vector_type right{};

		explicit AudioFileInMemoryFacade(AudioFileReader &reader) :
			inMemory{ reader } {}

		void readMonoFrames(vector_type::size_type n) {
			left.resize(n);
			std::vector<gsl::span<float>> mono{ left };
			inMemory.read(mono);
		}

		void readStereoFrames(vector_type::size_type n) {
			left.resize(n);
			right.resize(n);
			std::vector<gsl::span<float>> stereo{ left, right };
			inMemory.read(stereo);
		}

		bool complete() {
			return inMemory.complete();
		}

		long long framesRemaining() {
			return inMemory.framesRemaining();
		}

		void reset() {
			inMemory.reset();
		}
	};

	class AudioFileInMemoryTests : public ::testing::Test {
	protected:
		FakeAudioFileReader reader{};
	};

	TEST_F(AudioFileInMemoryTests, readFillsEachChannelStereo) {
		reader.setContents({ 1, 2, 3, 4, 5, 6 });
		reader.setChannels(2);
		AudioFileInMemoryFacade adapter{ reader };
		adapter.readStereoFrames(3);
		assertEqual({ 1, 3, 5 }, adapter.left);
		assertEqual({ 2, 4, 6 }, adapter.right);
	}

	TEST_F(AudioFileInMemoryTests, emptyFileDoesNotThrowException) {
		reader.setContents({});
		AudioFileInMemory adapter{ reader };
	}

	TEST_F(AudioFileInMemoryTests, readNothingWhenExhausted) {
		reader.setContents({ 3, 4 });
		AudioFileInMemoryFacade adapter{ reader };
		adapter.readMonoFrames(1);
		EXPECT_EQ(3, adapter.left.front());
		adapter.readMonoFrames(1);
		EXPECT_EQ(4, adapter.left.front());
		adapter.readMonoFrames(1);
		EXPECT_EQ(4, adapter.left.front());
	}

	TEST_F(AudioFileInMemoryTests, completeWhenExhausted) {
		reader.setContents({ 3, 4 });
		AudioFileInMemoryFacade adapter{ reader };
		adapter.readMonoFrames(1);
		EXPECT_FALSE(adapter.complete());
		adapter.readMonoFrames(1);
		EXPECT_TRUE(adapter.complete());
	}

	TEST_F(AudioFileInMemoryTests, returnsFramesRemaining) {
		reader.setContents({ 1, 2, 3 });
		AudioFileInMemoryFacade adapter{ reader };
		EXPECT_EQ(3, adapter.framesRemaining());
		adapter.readMonoFrames(1);
		EXPECT_EQ(2, adapter.framesRemaining());
		adapter.readMonoFrames(1);
		EXPECT_EQ(1, adapter.framesRemaining());
		adapter.readMonoFrames(1);
		EXPECT_EQ(0, adapter.framesRemaining());
	}

	TEST_F(AudioFileInMemoryTests, returnsFramesRemainingStereo) {
		reader.setContents({ 1, 2, 3, 4, 5, 6 });
		reader.setChannels(2);
		AudioFileInMemoryFacade adapter{ reader };
		EXPECT_EQ(3, adapter.framesRemaining());
		adapter.readStereoFrames(1);
		EXPECT_EQ(2, adapter.framesRemaining());
		adapter.readStereoFrames(1);
		EXPECT_EQ(1, adapter.framesRemaining());
		adapter.readStereoFrames(1);
		EXPECT_EQ(0, adapter.framesRemaining());
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
		std::shared_ptr<FakeAudioFileReaderFactory> factory =
			std::make_shared<FakeAudioFileReaderFactory>(reader);
		AudioFileInMemoryFactory adapter{ factory };

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
		assertEqual("a", factory->filePath());
	}
}