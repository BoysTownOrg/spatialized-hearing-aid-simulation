#include "AudioFileWriterAdapter.h"
#include <vector>

AudioFileWriterAdapter::AudioFileWriterAdapter(std::shared_ptr<AudioFileWriter> writer) :
	writer{ std::move(writer) } 
{
	if (this->writer->failed())
		throw FileError{ this->writer->errorMessage() };
}

void AudioFileWriterAdapter::write(gsl::span<channel_type> audio) {
	if (audio.size() == 0)
		return;

	auto smallestChannel = std::min_element(
		audio.begin(),
		audio.end(),
		[](channel_type a, channel_type b) { 
			return a.size() < b.size(); 
		}
	);
	const auto frames = smallestChannel->size();
	std::vector<channel_type::element_type> buffer{};
	for (int i = 0; i < frames; ++i)
		for (auto channel : audio)
			buffer.push_back(channel.at(i));
	if (buffer.size() > 0)
		writer->writeFrames(&buffer.front(), frames);
}

AudioFileWriterAdapterFactory::AudioFileWriterAdapterFactory(AudioFileFactory *factory) : 
	factory{ factory } {}

std::shared_ptr<AudioFrameWriter> AudioFileWriterAdapterFactory::make(
	std::string filePath,
    const AudioFrameWriter::AudioFormat &
) {
	try {
		return std::make_shared<AudioFileWriterAdapter>(
			factory->makeWriterUsingLastReaderFormat(std::move(filePath))
		);
	}
	catch (const AudioFileWriterAdapter::FileError &e) {
		throw CreateError{ e.what() };
	}
}
