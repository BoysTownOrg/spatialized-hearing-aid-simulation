#include "FakeAudioFile.h"
#include "assert-utility.h"
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <gtest/gtest.h>

namespace {
	class AudioFileInMemoryFacade {
		AudioFileInMemory inMemory;
	public:
		using channel_type = AudioFileInMemory::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;
		using size_type = buffer_type::size_type;
		buffer_type left{};
		buffer_type right{};

		explicit AudioFileInMemoryFacade(AudioFileReader &reader) :
			inMemory{ reader } {}

		void readMonoFrames(size_type n) {
			left.resize(n);
			std::vector<channel_type> mono{ left };
			inMemory.read(mono);
		}

		void readStereoFrames(size_type n) {
			left.resize(n);
			right.resize(n);
			std::vector<channel_type> stereo{ left, right };
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

	class AudioFileInMemoryTests : public ::testing::Test {
	protected:
		FakeAudioFileReader reader{};

		AudioFileInMemory construct() {
			return AudioFileInMemory{ reader };
		}

		AudioFileInMemoryFacade constructFacade() {
			return AudioFileInMemoryFacade{ reader };
		}
	};

	TEST_F(AudioFileInMemoryTests, emptyFileDoesNotThrowException) {
		reader.setContents({});
		auto inMemory = construct();
	}

	TEST_F(AudioFileInMemoryTests, readFillsChannelMono) {
		reader.setChannels(1);
		reader.setContents({ 1, 2, 3 });
		auto facade = constructFacade();
		facade.readMonoFrames(3);
		assertEqual({ 1, 2, 3 }, facade.left);
	}

	TEST_F(AudioFileInMemoryTests, readFillsEachChannelStereo) {
		reader.setChannels(2);
		reader.setContents({ 1, 2, 3, 4, 5, 6 });
		auto facade = constructFacade();
		facade.readStereoFrames(3);
		assertEqual({ 1, 3, 5 }, facade.left);
		assertEqual({ 2, 4, 6 }, facade.right);
	}

	TEST_F(AudioFileInMemoryTests, readNothingWhenExhausted) {
		reader.setContents({ 2 });
		auto facade = constructFacade();
		facade.readMonoFrames(1);
		assertEqual({ 2 }, facade.left);
		facade.readMonoFrames(1);
		assertEqual({ 2 }, facade.left);
	}

	TEST_F(AudioFileInMemoryTests, completeWhenExhausted) {
		reader.setContents({ 2, 3 });
		auto facade = constructFacade();
		facade.readMonoFrames(1);
		assertFalse(facade.complete());
		facade.readMonoFrames(1);
		assertTrue(facade.complete());
	}

	TEST_F(AudioFileInMemoryTests, completeWhenExhaustedReadingMoreThanOneSampleAtATime) {
		reader.setContents({ 3, 4, 5, 6 });
		auto facade = constructFacade();
		facade.readMonoFrames(2);
		assertFalse(facade.complete());
		facade.readMonoFrames(2);
		assertTrue(facade.complete());
	}

	TEST_F(AudioFileInMemoryTests, completeWhenExhaustedReadingBeyondContents) {
		reader.setContents({ 2, 3, 4, 5 });
		auto facade = constructFacade();
		facade.readMonoFrames(3);
		assertFalse(facade.complete());
		facade.readMonoFrames(3);
		assertTrue(facade.complete());
	}

	TEST_F(AudioFileInMemoryTests, remainingFramesUpdatesAfterReads) {
		reader.setContents({ 1, 2, 3 });
		auto facade = constructFacade();
		assertEqual(3LL, facade.remainingFrames());
		facade.readMonoFrames(1);
		assertEqual(2LL, facade.remainingFrames());
		facade.readMonoFrames(1);
		assertEqual(1LL, facade.remainingFrames());
		facade.readMonoFrames(1);
		assertEqual(0LL, facade.remainingFrames());
	}

	TEST_F(AudioFileInMemoryTests, remainingFramesUpdatesAfterReadsStereo) {
		reader.setChannels(2);
		reader.setContents({ 1, 2, 3, 4, 5, 6 });
		auto facade = constructFacade();
		assertEqual(3LL, facade.remainingFrames());
		facade.readStereoFrames(1);
		assertEqual(2LL, facade.remainingFrames());
		facade.readStereoFrames(1);
		assertEqual(1LL, facade.remainingFrames());
		facade.readStereoFrames(1);
		assertEqual(0LL, facade.remainingFrames());
	}

	TEST_F(AudioFileInMemoryTests, returnsFileParameters) {
		reader.setSampleRate(2);
		reader.setChannels(3);
		reader.setContents({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });
		AudioFileInMemory inMemory{ reader };
		assertEqual(12/3LL, inMemory.frames());
		assertEqual(3, inMemory.channels());
		assertEqual(2, inMemory.sampleRate());
	}

	TEST_F(AudioFileInMemoryTests, seeksBeginningOnResetMono) {
		reader.setChannels(1);
		reader.setContents({ 2, 3 });
		auto facade = constructFacade();
		facade.readMonoFrames(1);
		assertEqual({ 2 }, facade.left);
		facade.readMonoFrames(1);
		assertEqual({ 3 }, facade.left);
		facade.reset();
		facade.readMonoFrames(1);
		assertEqual({ 2 }, facade.left);
		facade.readMonoFrames(1);
		assertEqual({ 3 }, facade.left);
	}

	TEST_F(AudioFileInMemoryTests, seeksBeginningOnResetStereo) {
		reader.setChannels(2);
		reader.setContents({ 3, 4, 5, 6 });
		auto facade = constructFacade();
		facade.readStereoFrames(1);
		assertEqual({ 3 }, facade.left);
		assertEqual({ 4 }, facade.right);
		facade.readStereoFrames(1);
		assertEqual({ 5 }, facade.left);
		assertEqual({ 6 }, facade.right);
		facade.reset();
		facade.readStereoFrames(1);
		assertEqual({ 3 }, facade.left);
		assertEqual({ 4 }, facade.right);
		facade.readStereoFrames(1);
		assertEqual({ 5 }, facade.left);
		assertEqual({ 6 }, facade.right);
	}

	class AudioFileInMemoryFactoryTests : public ::testing::Test {
	protected:
		std::shared_ptr<FakeAudioFileReader> reader =
			std::make_shared<FakeAudioFileReader>();
		FakeAudioFileFactory factory{ reader };
		AudioFileInMemoryFactory inMemoryFactory{ &factory };

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
			inMemoryFactory.make(std::move(f));
		}
	};

	TEST_F(AudioFileInMemoryFactoryTests, factoryThrowsCreateErrorOnFileError) {
		reader->fail();
		reader->setErrorMessage("error.");
		assertMakeThrowsCreateError("error.");
	}

	TEST_F(AudioFileInMemoryFactoryTests, factoryPassesFilePath) {
		make("a");
		assertEqual("a", factory.filePathForReading());
	}
}