#pragma once

#ifdef BINAURAL_ROOM_IMPULSE_RESPONSE_EXPORTS
#define BINAURAL_ROOM_IMPULSE_RESPONSE_API __declspec(dllexport)
#else
#define BINAURAL_ROOM_IMPULSE_RESPONSE_API __declspec(dllimport)
#endif

#include <audio-file-reading/AudioFileReader.h>
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>

class BrirAdapter {
	std::shared_ptr<AudioFileReaderFactory> factory;
public:
	BINAURAL_ROOM_IMPULSE_RESPONSE_API explicit BrirAdapter(std::shared_ptr<AudioFileReaderFactory> factory);
	BINAURAL_ROOM_IMPULSE_RESPONSE_API BinauralRoomImpulseResponse read(std::string filePath);
};