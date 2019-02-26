#include "FakeAudioFile.h"
#include "assert-utility.h"
#include <audio-file-reading-writing/AudioFileWriterAdapter.h>
#include <gtest/gtest.h>
#include <vector>

namespace {
	class AudioFileWriterAdapterTests : public ::testing::Test {
	protected:
		using channel_type = AudioFileWriterAdapter::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;
		buffer_type left{};
		buffer_type right{};
		std::shared_ptr<FakeAudioFileWriter> writer = 
			std::make_shared<FakeAudioFileWriter>();
		AudioFileWriterAdapter adapter{ writer };

		void writeStereo() {
			std::vector<channel_type> channels{ left, right };
			writer->setChannels(2);
			adapter.write(channels);
		}
	};

	TEST_F(AudioFileWriterAdapterTests, writeInterleavesChannels) {
		left = { 1, 3, 5 };
		right = { 2, 4, 6 };
		writeStereo();
		assertEqual({ 1, 2, 3, 4, 5, 6 }, writer->written());
	}

	TEST_F(AudioFileWriterAdapterTests, writeEmptyChannelsDoesNotThrow) {
		left = {};
		right = {};
		writeStereo();
		assertTrue(writer->written().empty());
	}

	TEST_F(AudioFileWriterAdapterTests, writeTruncatesToSmallestChannel) {
		left = { 1, 3, 5 };
		right = { 2, 4 };
		writeStereo();
		assertEqual({ 1, 2, 3, 4 }, writer->written());
	}

	TEST_F(AudioFileWriterAdapterTests, writeEmptyAudioDoesNotThrow) {
		std::vector<channel_type> channels{};
		adapter.write(channels);
		assertTrue(writer->written().empty());
	}

	class AudioFileWriterAdapterFactoryTests : public ::testing::Test {
	protected:
		std::shared_ptr<FakeAudioFileWriter> writer =
			std::make_shared<FakeAudioFileWriter>();
		FakeAudioFileFactory factory{ writer };
		AudioFileWriterAdapterFactory adapterFactory{ &factory };

		void assertMakeThrowsCreateError(std::string what) {
			try {
				make();
				FAIL() << "Expected AudioFileWriterAdapterFactory::CreateError";
			}
			catch (const AudioFileWriterAdapterFactory::CreateError &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void make(std::string f = {}) {
			adapterFactory.make(std::move(f));
		}
	};

	TEST_F(AudioFileWriterAdapterFactoryTests, throwsCreateErrorOnFileError) {
		writer->fail();
		writer->setErrorMessage("error.");
		assertMakeThrowsCreateError("error.");
	}

	TEST_F(AudioFileWriterAdapterFactoryTests, passesFilePath) {
		make("a");
		assertEqual("a", factory.filePathForWriting());
	}
}