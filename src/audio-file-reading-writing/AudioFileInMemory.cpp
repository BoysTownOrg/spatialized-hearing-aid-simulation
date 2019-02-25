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
	if (buffer.size())
		reader.readFrames(&buffer.front(), reader.frames());
}

void AudioFileInMemory::read(gsl::span<channel_type> audio) {
	if (audio.size() != channels_)
		return;
	size_type samples{ 0 };
	for (int i{ 0 }; i < channels_; ++i) {
		auto channel = audio[i];
		samples = std::min(gsl::narrow<size_type>(channel.size()), remainingFrames_());
		for (size_type j{ 0 }; j < samples; ++j)
			channel.at(j) = buffer.at(head + i + j * channels_);
	}
	head += samples * channels_;
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

auto AudioFileInMemory::remainingFrames_() -> size_type {
	return (buffer.size() - head) / channels_;
}

AudioFileInMemoryFactory::AudioFileInMemoryFactory(
	AudioFileFactory *factory
) noexcept :
	factory{ factory } {}

std::shared_ptr<AudioFrameReader> AudioFileInMemoryFactory::make(
	std::string filePath
) {
	try {
		return std::make_shared<AudioFileInMemory>(
			*factory->makeReader(std::move(filePath))
		);
	}
	catch (const AudioFileInMemory::FileError &e) {
		throw CreateError{ e.what() };
	}
}
