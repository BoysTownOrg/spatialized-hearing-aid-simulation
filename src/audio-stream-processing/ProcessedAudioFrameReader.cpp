#include "ProcessedAudioFrameReader.h"

ProcessedAudioFrameReader::ProcessedAudioFrameReader(
	std::shared_ptr<AudioFrameReader> reader,
	std::shared_ptr<AudioProcessor> processor
) :
	reader{ std::move(reader) },
	processor{ std::move(processor) } {}

void ProcessedAudioFrameReader::read(float ** frames, int frameCount) {
	reader->read(frames, frameCount);
	processor->process(frames, frameCount);
}
