#include "BrirAdapter.h"
#include <gsl/gsl>
#include <algorithm>

BrirAdapter::BrirAdapter(std::shared_ptr<AudioFileReaderFactory> factory) :
	factory{ std::move(factory) }
{
}

auto BrirAdapter::read(std::string filePath) -> BinauralRoomImpulseResponse {
	return makeBrir(*makeReader(std::move(filePath)));
}

std::shared_ptr<AudioFileReader> BrirAdapter::makeReader(std::string filePath) {
	const auto reader = factory->make(std::move(filePath));
	if (reader->failed())
		throw ReadError{ reader->errorMessage() };
	return reader;
}

auto BrirAdapter::makeBrir(AudioFileReader &reader) -> BinauralRoomImpulseResponse {
	BinauralRoomImpulseResponse brir{};
	brir.sampleRate = reader.sampleRate();
	vector_type buffer(gsl::narrow<vector_type::size_type>(reader.frames() * reader.channels()));
	if (buffer.size()) {
		reader.readFrames(&buffer.front(), reader.frames());
		for (vector_type::size_type i = 0; i < reader.frames(); ++i)
			brir.left.push_back(buffer[i * reader.channels()]);
		if (reader.channels() > 1)
			for (vector_type::size_type i = 0; i < reader.frames(); ++i)
				brir.right.push_back(buffer[i * reader.channels() + 1]);
	}
	return brir;
}

