#include "StereoAudioFileAdapter.h"

StereoAudioFileAdapter::StereoAudioFileAdapter(
	std::shared_ptr<StereoAudioFile> file
) :
	file{ std::move(file) } {}

void StereoAudioFileAdapter::read(float ** channels, int frameCount) {
	file->read(channels[0], channels[1], frameCount);
}
