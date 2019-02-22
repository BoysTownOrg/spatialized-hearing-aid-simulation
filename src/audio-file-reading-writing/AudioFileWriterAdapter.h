#pragma once

#include <common-includes/Interface.h>
#include <memory>
#include <string>

class AudioFileWriter {
public:
	INTERFACE_OPERATIONS(AudioFileWriter);
	virtual void writeFrames(float *, long long) = 0;
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
};

class AudioFileWriterFactory {
public:
	INTERFACE_OPERATIONS(AudioFileWriterFactory);
	virtual std::shared_ptr<AudioFileWriter> make(std::string filePath) = 0;
};

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
	AudioFileWriterFactory *factory;
public:
	AUDIO_FILE_READING_WRITING_API explicit AudioFileWriterAdapterFactory(AudioFileWriterFactory *);

	AUDIO_FILE_READING_WRITING_API std::shared_ptr<AudioFileWriterAdapter> make(std::string filePath);
	RUNTIME_ERROR(CreateError);
};