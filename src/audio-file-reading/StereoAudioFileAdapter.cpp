#include "StereoAudioFileAdapter.h"

StereoAudioFileAdapter::StereoAudioFileAdapter(
	std::shared_ptr<StereoAudioFile> file
) :
	file{ std::move(file) } {}

void StereoAudioFileAdapter::read(float **frames, int frameCount) {
	file->read(frames[0], frames[1], frameCount);
}
