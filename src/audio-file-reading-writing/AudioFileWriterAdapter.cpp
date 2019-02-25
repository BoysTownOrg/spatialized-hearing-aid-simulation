#include "AudioFileWriterAdapter.h"

AudioFileWriterAdapter::AudioFileWriterAdapter(std::shared_ptr<AudioFileWriter> writer) :
	writer{ std::move(writer) } 
{
	if (this->writer->failed())
		throw FileError{ this->writer->errorMessage() };
}

void AudioFileWriterAdapter::write(gsl::span<channel_type> audio) {
	std::vector<channel_type::element_type> buffer{};
	for (int i = 0; i < audio.begin()->size(); ++i)
		for (auto channel : audio)
			buffer.push_back(channel.at(i));
	writer->writeFrames(&buffer.front(), buffer.size());
}

AudioFileWriterAdapterFactory::AudioFileWriterAdapterFactory(AudioFileWriterFactory *factory) : 
	factory{ factory } {}

std::shared_ptr<AudioFileWriterAdapter> AudioFileWriterAdapterFactory::make(
	std::string filePath,
	std::string formatMatchedFilePath
) {
	try {
		return std::make_shared<AudioFileWriterAdapter>(
			factory->make(std::move(filePath), std::move(formatMatchedFilePath))
		);
	}
	catch (const AudioFileWriterAdapter::FileError &e) {
		throw CreateError{ e.what() };
	}
}
