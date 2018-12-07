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
	using vector_type = decltype(brir.left);
	vector_type buffer(gsl::narrow<vector_type::size_type>(reader->frames() * reader->channels()));
	if (buffer.size())
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
