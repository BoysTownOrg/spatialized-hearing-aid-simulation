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

void AudioFileInMemory::read(gsl::span<gsl::span<float>> audio) {
	for (int i = 0; i < audio.begin()->size(); ++i)
		for (const auto channel : audio) {
			if (head == buffer.size())
				return;
			channel[i] = buffer[head++];
		}
}

bool AudioFileInMemory::complete() const {
	return head == buffer.size();
}

int AudioFileInMemory::sampleRate() const {
	return sampleRate_;
}

int AudioFileInMemory::channels() const {
	return channels_;
}

long long AudioFileInMemory::frames() const {
	return frames_;
}

void AudioFileInMemory::reset() {
	head = 0;
}

AudioFileInMemoryFactory::AudioFileInMemoryFactory(
	std::shared_ptr<AudioFileReaderFactory> factory
) :
	factory{ std::move(factory) }
{
}

std::shared_ptr<AudioFrameReader> AudioFileInMemoryFactory::make(std::string filePath) {
	try {
		return std::make_shared<AudioFileInMemory>(*factory->make(filePath));
	}
	catch (const AudioFileInMemory::FileError &e) {
		throw CreateError{ e.what() };
	}
}
