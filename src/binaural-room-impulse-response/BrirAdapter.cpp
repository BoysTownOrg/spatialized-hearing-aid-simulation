#include "BrirAdapter.h"
#include <gsl/gsl>
#include <algorithm>

class ChannelReader {
	std::vector<float> contents;
	int channels;
public:
	ChannelReader(std::vector<float> contents, int channels) :
		contents{ std::move(contents) },
		channels{ channels } {}

	std::vector<float> read(int channel) {
		std::vector<float> x{};
		std::vector<float>::size_type i = channel;
		while (i < contents.size()) {
			x.push_back(contents[i]);
			i += channels;
		}
		return x;
	}
};

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
		ChannelReader channelReader{ buffer, reader.channels() };
		brir.left = channelReader.read(0);
		if (reader.channels() > 1)
			brir.right = channelReader.read(1);
	}
	return brir;
}

