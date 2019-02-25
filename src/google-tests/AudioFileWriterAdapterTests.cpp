#include "assert-utility.h"
#include <audio-file-reading-writing/AudioFileWriterAdapter.h>
#include <gtest/gtest.h>
#include <vector>

class FakeAudioFileWriter : public AudioFileWriter {
	std::vector<float> written_{};
	std::string errorMessage_{};
	bool failed_{};
public:
	void fail() {
		failed_ = true;
	}

	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	auto written() {
		return written_;
	}

	void writeFrames(float *x, long long n) override {
		gsl::span<float> audio{ x, gsl::narrow<gsl::span<float>::index_type>(n) };
		std::copy(audio.begin(), audio.end(), std::back_inserter(written_));
	}

	bool failed() override {
		return failed_;
	}

	std::string errorMessage() override {
		return errorMessage_;
	}
};

class FakeAudioFileWriterFactory : public AudioFileWriterFactory {
	std::string filePath_{};
	std::shared_ptr<AudioFileWriter> writer;
public:
	explicit FakeAudioFileWriterFactory(
		std::shared_ptr<AudioFileWriter> writer
	) noexcept :
		writer{ std::move(writer) } {}

	std::shared_ptr<AudioFileWriter> make(std::string filePath) override {
		filePath_ = std::move(filePath);
		return writer;
	}

	auto filePath() const {
		return filePath_;
	}
};

namespace {
	class AudioFileWriterAdapterTests : public ::testing::Test {
	protected:
		std::shared_ptr<FakeAudioFileWriter> writer = 
			std::make_shared<FakeAudioFileWriter>();
		AudioFileWriterAdapter adapter{ writer };
	};

	TEST_F(AudioFileWriterAdapterTests, writeInterleavesChannels) {
		std::vector<float> left{ 1, 3, 5 };
		std::vector<float> right{ 2, 4, 6 };
		std::vector<gsl::span<float>> channels = { left, right };
		adapter.write(channels);
		assertEqual({ 1, 2, 3, 4, 5, 6 }, writer->written());
	}

	class AudioFileWriterAdapterFactoryTests : public ::testing::Test {
	protected:
		std::shared_ptr<FakeAudioFileWriter> writer =
			std::make_shared<FakeAudioFileWriter>();
		FakeAudioFileWriterFactory factory{ writer };
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
		assertEqual("a", factory.filePath());
	}
}