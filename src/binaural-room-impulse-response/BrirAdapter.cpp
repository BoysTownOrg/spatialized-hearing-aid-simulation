#include "BrirAdapter.h"
#include <gsl/gsl>

class BrirAdapter::ChannelReader {
	vector_type contents;
	int channels;
public:
	explicit ChannelReader(AudioFileReader &reader) :
		contents(gsl::narrow<vector_type::size_type>(reader.frames() * reader.channels())),
		channels{ reader.channels() } 
	{
		if (contents.size())
			reader.readFrames(&contents.front(), reader.frames());
	}

	vector_type read(vector_type::size_type channel) {
		vector_type result{};
		auto i{ channel };
		while (i < contents.size()) {
			result.push_back(contents[i]);
			i += channels;
		}
		return result;
	}

	bool empty() {
		return contents.empty();
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
		throw ReadFailure{ reader->errorMessage() };
	return reader;
}

auto BrirAdapter::makeBrir(AudioFileReader &reader) -> BinauralRoomImpulseResponse {
	BinauralRoomImpulseResponse brir{};
	ChannelReader channelReader{ reader };
	if (!channelReader.empty()) {
		brir.left = channelReader.read(0);
		if (reader.channels() > 1)
			brir.right = channelReader.read(1);
	}
	brir.sampleRate = reader.sampleRate();
	return brir;
}

