#pragma once

#include <audio-file-reading-writing/AudioFile.h>
#include <audio-file-reading-writing/AudioFileWriterAdapter.h>
#include <sndfile.h>

class LibsndfileReader : public AudioFileReader {
	SF_INFO info{};
	SNDFILE *file{};
public:
	explicit LibsndfileReader(std::string filePath);
	~LibsndfileReader() noexcept override;
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
	const SF_INFO &sfInfo() noexcept { return info; }
};

class LibsndfileWriter : public AudioFileWriter {
	SNDFILE *file{};
public:
	LibsndfileWriter(std::string filePath, const AudioFileWriter::AudioFileFormat &format);
	~LibsndfileWriter() noexcept override;
	LibsndfileWriter(LibsndfileWriter &&) = delete;
	LibsndfileWriter &operator=(LibsndfileWriter &&) = delete;
	LibsndfileWriter(const LibsndfileWriter &) = delete;
	LibsndfileWriter &operator=(const LibsndfileWriter &) = delete;
	void writeFrames(float *, long long) override;
	bool failed() override;
	std::string errorMessage() override;
};

class LibsndfileFactory : public AudioFileFactory {
public:
	std::shared_ptr<AudioFileReader> makeReader(std::string filePath) override;
	std::shared_ptr<AudioFileWriter> makeWriter(
        std::string filePath,
        const AudioFileWriter::AudioFileFormat &
    ) override;
};
