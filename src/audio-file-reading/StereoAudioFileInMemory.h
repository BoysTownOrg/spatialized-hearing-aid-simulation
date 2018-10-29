#pragma once

#include "audio-file-reading-exports.h"
#include "AudioFileReader.h"
#include <memory>
#include <vector>

class StereoAudioFileInMemory {
	std::vector<float> buffer;
	int channels;
	std::size_t head = 0;
public:
	class InvalidChannelCount {};
	AUDIO_FILE_READING_API explicit StereoAudioFileInMemory(
		std::shared_ptr<AudioFileReader> reader
	);
	AUDIO_FILE_READING_API int framesRemaining() const;
	AUDIO_FILE_READING_API void read(float *left, float *right, int samples);
};

