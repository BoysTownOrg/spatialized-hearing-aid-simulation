#pragma once

#include <spatialized-hearing-aid-simulation/BrirReader.h>
#include <audio-file-reading-writing/AudioFile.h>

#ifdef BINAURAL_ROOM_IMPULSE_RESPONSE_EXPORTS
	#define BINAURAL_ROOM_IMPULSE_RESPONSE_API __declspec(dllexport)
#else
	#define BINAURAL_ROOM_IMPULSE_RESPONSE_API __declspec(dllimport)
#endif

class BrirAdapter : public BrirReader {
	AudioFileFactory *factory;
public:
	BINAURAL_ROOM_IMPULSE_RESPONSE_API 
		explicit BrirAdapter(AudioFileFactory *) noexcept;
	BINAURAL_ROOM_IMPULSE_RESPONSE_API 
		BinauralRoomImpulseResponse read(std::string filePath) override;
private:
	class ChannelReader;
	std::shared_ptr<AudioFileReader> makeReader(std::string filePath);
	BinauralRoomImpulseResponse makeBrir(AudioFileReader &);
};