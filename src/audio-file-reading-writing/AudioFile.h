#pragma once

#include <common-includes/Interface.h>
#include <string>
#include <memory>

class AudioFileReader {
public:
    INTERFACE_OPERATIONS(AudioFileReader)
	virtual long long frames() = 0;
	virtual int channels() = 0;
	virtual int sampleRate() = 0;
	virtual void readFrames(float *, long long) = 0;
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
};

class AudioFileWriter {
public:
    INTERFACE_OPERATIONS(AudioFileWriter)
    struct AudioFileFormat {
        int channels;
        int sampleRate;
    };
	virtual void writeFrames(float *, long long) = 0;
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
};

class AudioFileFactory {
public:
    INTERFACE_OPERATIONS(AudioFileFactory)
	virtual std::shared_ptr<AudioFileReader> makeReader(std::string filePath) = 0;
	virtual std::shared_ptr<AudioFileWriter> makeWriter(
        std::string filePath,
        const AudioFileWriter::AudioFileFormat &
    ) = 0;
};

