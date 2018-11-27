#pragma once

#include "audio-file-reading-exports.h"
#include <audio-file-reading/AudioFileReader.h>
#include <audio-stream-processing/AudioFrameReader.h>
#include <vector>

class AudioFileInMemory : public AudioFrameReader {
	using size_type = std::vector<float>::size_type;
	std::vector<float> buffer;
	int channelCount;
	size_type head = 0;
public:
	AUDIO_FILE_READING_API explicit AudioFileInMemory(
		std::shared_ptr<AudioFileReader> reader
	);
	AUDIO_FILE_READING_API void read(float ** channels, int frameCount) override;
};

