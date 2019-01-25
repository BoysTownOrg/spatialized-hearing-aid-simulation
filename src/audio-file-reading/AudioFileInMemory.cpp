#include "AudioFileInMemory.h"
#include <gsl/gsl>

AudioFileInMemory::AudioFileInMemory(AudioFileReader &reader) :
	buffer(gsl::narrow<size_type>(reader.frames() * reader.channels())),
	frames_{ reader.frames() },
	channels_{ reader.channels() },
	sampleRate_{ reader.sampleRate() }
{
	if (reader.failed())
		throw FileError{ reader.errorMessage() };
	if (buffer.size() == 0)
		return;
	reader.readFrames(&buffer.front(), reader.frames());
}

void AudioFileInMemory::read(gsl::span<channel_type> audio) {
	auto channels = audio.size();
	for (channel_type::index_type i = 0; i < channels; ++i) {
		auto channel = audio[i];
		auto samples = std::min(gsl::narrow<size_type>(channel.size()), remainingFrames_());
		for (size_type j = 0; j < samples; ++j)
			channel[j] = buffer[head + i + j * channels_];
	}
	if (!complete_())
		head += channels;
}

bool AudioFileInMemory::complete() {
	return complete_();
}

bool AudioFileInMemory::complete_() {
	return head == buffer.size();
}

int AudioFileInMemory::sampleRate() {
	return sampleRate_;
}

int AudioFileInMemory::channels() {
	return channels_;
}

long long AudioFileInMemory::frames() {
	return frames_;
}

void AudioFileInMemory::reset() {
	head = 0;
}

long long AudioFileInMemory::remainingFrames() {
	return remainingFrames_();
}

unsigned int AudioFileInMemory::remainingFrames_() {
	return (buffer.size() - head) / channels_;
}

AudioFileInMemoryFactory::AudioFileInMemoryFactory(
	std::shared_ptr<AudioFileReaderFactory> factory
) :
	factory{ std::move(factory) }
{
}

std::shared_ptr<AudioFrameReader> AudioFileInMemoryFactory::make(
	std::string filePath
) {
	try {
		return std::make_shared<AudioFileInMemory>(
			*factory->make(std::move(filePath))
		);
	}
	catch (const AudioFileInMemory::FileError &e) {
		throw CreateError{ e.what() };
	}
}
