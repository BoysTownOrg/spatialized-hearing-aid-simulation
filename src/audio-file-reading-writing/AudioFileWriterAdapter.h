#pragma once

#include "AudioFile.h"
#include "audio-file-reading-writing-exports.h"
#include <spatialized-hearing-aid-simulation/AudioFrameWriter.h>
#include <common-includes/RuntimeError.h>

class AudioFileWriterAdapter : public AudioFrameWriter {
	std::shared_ptr<AudioFileWriter> writer;
public:
	AUDIO_FILE_READING_WRITING_API explicit AudioFileWriterAdapter(std::shared_ptr<AudioFileWriter>);
	RUNTIME_ERROR(FileError);
	AUDIO_FILE_READING_WRITING_API void write(gsl::span<channel_type> audio) override;
};

class AudioFileWriterAdapterFactory : public AudioFrameWriterFactory {
	AudioFileFactory *factory;
public:
	AUDIO_FILE_READING_WRITING_API explicit AudioFileWriterAdapterFactory(AudioFileFactory *);
	AUDIO_FILE_READING_WRITING_API std::shared_ptr<AudioFrameWriter> make(std::string filePath) override;
};