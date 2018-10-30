#pragma once

#include <audio-file-reading/StereoAudioFileInMemory.h>
#include <sndfile.h>

class LibsndfileReader : public AudioFileReader {
	SNDFILE *file{};
	SF_INFO info{};
public:
	explicit LibsndfileReader(std::string filePath);
	~LibsndfileReader();
	void readFrames(float *, long long) override;
	long long frames() override;
	int channels() override;
};
