#include "BrirAdapter.h"
#include <gsl/gsl>

class BrirAdapter::ChannelReader {
	impulse_response_type contents;
	using size_type = impulse_response_type::size_type;
	int channels;
public:
	explicit ChannelReader(AudioFileReader &reader) :
		contents(gsl::narrow<size_type>(reader.frames() * reader.channels())),
		channels{ reader.channels() } 
	{
		if (contents.size())
			reader.readFrames(&contents.front(), reader.frames());
	}

	impulse_response_type read(size_type channel) {
		impulse_response_type result{};
		auto i{ channel };
		while (i < contents.size()) {
			result.push_back(contents.at(i));
			i += channels;
		}
		return result;
	}

	bool empty() noexcept {
		return contents.empty();
	}
};

BrirAdapter::BrirAdapter(AudioFileReaderFactory *factory) noexcept :
	factory{ factory } {}

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
		const auto leftChannel = 0;
		brir.left = channelReader.read(leftChannel);
		if (reader.channels() > 1) {
			const auto rightChannel = 1;
			brir.right = channelReader.read(rightChannel);
		}
	}
	brir.sampleRate = reader.sampleRate();
	return brir;
}

