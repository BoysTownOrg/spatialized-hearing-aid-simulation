#include "BrirAdapter.h"
#include <gsl/gsl>
#include <algorithm>

BrirAdapter::BrirAdapter(std::shared_ptr<AudioFileReaderFactory> factory) :
	factory{ std::move(factory) }
{
}

auto BrirAdapter::read(std::string filePath) -> BinauralRoomImpulseResponse {
	const auto reader = factory->make(filePath);
	if (reader->failed())
		throw ReadError{ reader->errorMessage() };
	BinauralRoomImpulseResponse brir{};
	brir.sampleRate = reader->sampleRate();
	using vector_type = decltype(brir.left);
	vector_type buffer(gsl::narrow<vector_type::size_type>(reader->frames() * reader->channels()));
	if (buffer.size()) {
		reader->readFrames(&buffer[0], reader->frames());
		for (vector_type::size_type i = 0; i < reader->frames(); ++i)
			brir.left.push_back(buffer[i * reader->channels()]);
		if (reader->channels() > 1)
			for (vector_type::size_type i = 0; i < reader->frames(); ++i)
				brir.right.push_back(buffer[i * reader->channels() + 1]);
	}
	return brir;
}
