#include "ProcessedAudioStream.h"

ProcessedAudioStream::ProcessedAudioStream(
	std::shared_ptr<AudioReader> reader, 
	std::shared_ptr<AudioProcessor> processor
) :
	reader{ std::move(reader) },
	processor{ std::move(processor) } {}

void ProcessedAudioStream::fillBuffer(float ** channels, int) {
	reader->read(channels, 0);
	processor->process(channels, 0);
}
