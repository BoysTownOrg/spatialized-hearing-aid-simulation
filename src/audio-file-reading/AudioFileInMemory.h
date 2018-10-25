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
	std::vector<float> left;
	std::vector<float> right;
	std::size_t leftHead = 0;
	std::size_t rightHead = 0;
public:
	AUDIO_FILE_READING_API explicit AudioFileInMemory(
		std::shared_ptr<AudioFileReader> reader
	);
	AUDIO_FILE_READING_API std::vector<float> readLeftChannel(int samples);
	AUDIO_FILE_READING_API std::vector<float> readRightChannel(int samples);
};

