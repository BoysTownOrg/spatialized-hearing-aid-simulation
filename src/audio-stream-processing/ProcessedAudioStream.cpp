#include "ProcessedAudioStream.h"

ProcessedAudioStream::ProcessedAudioStream(
	std::shared_ptr<AudioFrameReader> reader,
	std::shared_ptr<AudioProcessor> processor
) :
	reader{ std::move(reader) },
	processor{ std::move(processor) } {}

void ProcessedAudioStream::read(float ** channels, int frameCount) {
	reader->read(channels, frameCount);
	processor->process(channels, frameCount);
}
