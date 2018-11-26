#pragma once

#include <audio-file-reading/StereoAudioFileInMemory.h>
#include <sndfile.h>

class LibsndfileReader : public AudioFileReader {
	SNDFILE *file{};
	SF_INFO info{};
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
};

class LibsndfileReaderFactory : public AudioFileReaderFactory {
	std::shared_ptr<AudioFileReader> make(std::string filePath) override {
		return std::make_shared<LibsndfileReader>(filePath);
	}
};