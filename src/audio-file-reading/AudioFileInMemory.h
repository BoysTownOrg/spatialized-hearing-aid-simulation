#pragma once

#include "audio-file-reading-exports.h"
#include "AudioFileReader.h"
#include <common-includes/RuntimeError.h>
#include <audio-stream-processing/AudioFrameReader.h>
#include <vector>

class AudioFileInMemory : public AudioFrameReader {
	using buffer_type = std::vector<float>;
	using size_type = buffer_type::size_type;
	buffer_type buffer;
	int channelCount;
	int _sampleRate;
	size_type head = 0;
public:
	RUNTIME_ERROR(FileError);
	AUDIO_FILE_READING_API explicit AudioFileInMemory(
		AudioFileReader &reader
	);
	AUDIO_FILE_READING_API void read(float ** channels, int frameCount) override;
	AUDIO_FILE_READING_API bool complete() const override;
	AUDIO_FILE_READING_API int sampleRate() const override;
};
