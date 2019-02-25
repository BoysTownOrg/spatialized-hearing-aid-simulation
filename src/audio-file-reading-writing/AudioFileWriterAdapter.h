#pragma once

#include "AudioFile.h"
#include "audio-file-reading-writing-exports.h"
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>

class AudioFileWriterAdapter {
	std::shared_ptr<AudioFileWriter> writer;
public:
	AUDIO_FILE_READING_WRITING_API explicit AudioFileWriterAdapter(std::shared_ptr<AudioFileWriter>);
	RUNTIME_ERROR(FileError);

	using channel_type = gsl::span<float>;
	AUDIO_FILE_READING_WRITING_API void write(gsl::span<channel_type> audio);
};

class AudioFileWriterAdapterFactory {
	AudioFileFactory *factory;
public:
	AUDIO_FILE_READING_WRITING_API explicit AudioFileWriterAdapterFactory(AudioFileFactory *);

	AUDIO_FILE_READING_WRITING_API std::shared_ptr<AudioFileWriterAdapter> make(std::string filePath);
	RUNTIME_ERROR(CreateError);
};