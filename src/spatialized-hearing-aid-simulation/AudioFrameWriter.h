#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <memory>

class AudioFrameWriter {
public:
	using channel_type = gsl::span<float>;
	INTERFACE_OPERATIONS(AudioFrameWriter);
	virtual void write(gsl::span<channel_type> audio) = 0;
};

class AudioFrameWriterFactory {
public:
	INTERFACE_OPERATIONS(AudioFrameWriterFactory);
	virtual std::shared_ptr<AudioFrameWriter> make(std::string filePath) = 0;
	RUNTIME_ERROR(CreateError);
};