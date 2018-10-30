#pragma once

#include "audio-file-reading-exports.h"
#include "StereoAudioFile.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <memory>

class StereoAudioFileAdapter : public AudioFrameReader {
	std::shared_ptr<StereoAudioFile> file;
public:
	AUDIO_FILE_READING_API explicit StereoAudioFileAdapter(
		std::shared_ptr<StereoAudioFile> file);
	AUDIO_FILE_READING_API void read(float **, int) override;
};

