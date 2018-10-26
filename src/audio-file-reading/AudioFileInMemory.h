#pragma once

#ifdef AUDIO_FILE_READING_EXPORTS
	#define AUDIO_FILE_READING_API __declspec(dllexport)
#else
	#define AUDIO_FILE_READING_API __declspec(dllimport)
#endif

#include "AudioFileReader.h"
#include <memory>
#include <vector>

class AudioFileInMemory {
	std::vector<float> buffer;
	std::size_t head = 0;
public:
	class InvalidChannelCount {};
	AUDIO_FILE_READING_API explicit AudioFileInMemory(
		std::shared_ptr<AudioFileReader> reader
	);
	AUDIO_FILE_READING_API int samplesRemaining();
	AUDIO_FILE_READING_API void read(float *left, float *right, int samples);
};

