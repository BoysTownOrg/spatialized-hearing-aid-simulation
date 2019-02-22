#include "AudioFileWriterAdapter.h"

AudioFileWriterAdapter::AudioFileWriterAdapter(std::shared_ptr<AudioFileWriter> writer) :
	writer{ std::move(writer) } {}

void AudioFileWriterAdapter::write(gsl::span<channel_type> audio) {
	std::vector<channel_type::element_type> buffer{};
	for (int i = 0; i < audio.begin()->size(); ++i)
		for (auto channel : audio)
			buffer.push_back(channel.at(i));
	writer->writeFrames(&buffer.front(), buffer.size());
}
