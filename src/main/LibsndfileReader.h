#pragma once

#include <audio-file-reading-writing/AudioFileReader.h>
#include <audio-file-reading-writing/AudioFileWriterAdapter.h>
#include <sndfile.h>

class LibsndfileReader : public AudioFileReader {
	SF_INFO info{};
	SNDFILE *file{};
public:
	explicit LibsndfileReader(std::string filePath);
	~LibsndfileReader() noexcept;
	LibsndfileReader(LibsndfileReader &&) = delete;
	LibsndfileReader &operator=(LibsndfileReader &&) = delete;
	LibsndfileReader(const LibsndfileReader &) = delete;
	LibsndfileReader &operator=(const LibsndfileReader &) = delete;
	void readFrames(float *, long long) override;
	long long frames() override;
	int channels() override;
	bool failed() override;
	std::string errorMessage() override;
	int sampleRate() override;
};

class LibsndfileReaderFactory : public AudioFileReaderFactory {
	std::shared_ptr<AudioFileReader> make(std::string filePath) override {
		return std::make_shared<LibsndfileReader>(std::move(filePath));
	}
};

class LibsndfileWriter : public AudioFileWriter {
	SF_INFO info{};
	SNDFILE *file{};
public:
	explicit LibsndfileWriter(std::string filePath);
	~LibsndfileWriter() noexcept;
	LibsndfileWriter(LibsndfileWriter &&) = delete;
	LibsndfileWriter &operator=(LibsndfileWriter &&) = delete;
	LibsndfileWriter(const LibsndfileWriter &) = delete;
	LibsndfileWriter &operator=(const LibsndfileWriter &) = delete;
	void writeFrames(float *, long long) override;
	bool failed() override;
	std::string errorMessage() override;
};

class LibsndfileWriterFactory : public AudioFileWriterFactory {
	std::shared_ptr<AudioFileWriter> make(std::string filePath) override {
		return std::make_shared<LibsndfileWriter>(std::move(filePath));
	}
};