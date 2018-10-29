#include "ProcessedAudioFrameReader.h"

ProcessedAudioFrameReader::ProcessedAudioFrameReader(
	std::shared_ptr<AudioFrameReader> reader,
	std::shared_ptr<AudioFrameProcessor> processor
) :
	reader{ std::move(reader) },
	processor{ std::move(processor) } {}

void ProcessedAudioFrameReader::read(float ** channels, int frameCount) {
	reader->read(channels, frameCount);
	processor->process(channels, frameCount);
}
