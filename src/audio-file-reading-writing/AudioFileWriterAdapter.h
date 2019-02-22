#pragma once

#include <common-includes/Interface.h>
#include <memory>

class AudioFileWriter {
public:
	INTERFACE_OPERATIONS(AudioFileWriter);
	virtual void writeFrames(float *, long long) = 0;
};

#include "audio-file-reading-writing-exports.h"
#include <gsl/gsl>

class AudioFileWriterAdapter {
	std::shared_ptr<AudioFileWriter> writer;
public:
	AUDIO_FILE_READING_WRITING_API explicit AudioFileWriterAdapter(std::shared_ptr<AudioFileWriter>);
	
	using channel_type = gsl::span<float>;
	AUDIO_FILE_READING_WRITING_API void write(gsl::span<channel_type> audio);
};