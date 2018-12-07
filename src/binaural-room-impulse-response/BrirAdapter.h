#pragma once

#include <audio-file-reading/AudioFileReader.h>
#include <dsl-prescription/ConfigurationFileParser.h>
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <gsl/gsl>

class BrirAdapter {
	std::shared_ptr<AudioFileReaderFactory> factory;
public:
	explicit BrirAdapter(std::shared_ptr<AudioFileReaderFactory> factory) :
		factory{ std::move(factory) }
	{
	}

	BinauralRoomImpulseResponse read(std::string filePath) {
		BinauralRoomImpulseResponse brir{};
		const auto reader = factory->make(filePath);
		using vector_type = decltype(brir.left);
		vector_type buffer(gsl::narrow<vector_type::size_type>(reader->frames() * reader->channels()));
		reader->readFrames(&buffer[0], reader->frames());
		bool oddSample = false;
		std::partition_copy(
			buffer.begin(),
			buffer.end(),
			std::back_inserter(brir.left),
			std::back_inserter(brir.right),
			[&oddSample](double) { return oddSample = !oddSample; });
		brir.sampleRate = reader->sampleRate();
		return brir;
	}
};