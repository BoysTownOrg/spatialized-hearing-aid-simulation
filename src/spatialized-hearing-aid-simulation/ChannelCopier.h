#pragma once

#include "AudioFrameReader.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <memory>

class ChannelCopier : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
public:
	SPATIALIZED_HA_SIMULATION_API explicit ChannelCopier(
		std::shared_ptr<AudioFrameReader>
	) noexcept;
	SPATIALIZED_HA_SIMULATION_API void read(gsl::span<channel_type> audio) override;
	SPATIALIZED_HA_SIMULATION_API bool complete() override;
	SPATIALIZED_HA_SIMULATION_API int sampleRate() override;
	SPATIALIZED_HA_SIMULATION_API int channels() override;
	SPATIALIZED_HA_SIMULATION_API long long frames() override;
	SPATIALIZED_HA_SIMULATION_API void reset() override;
    long long remainingFrames() override;
private:
	void readAndCopyFirstChannel(gsl::span<channel_type> audio);
	void readAllChannels(gsl::span<channel_type> audio);
	bool mono();
};

class ChannelCopierFactory : public AudioFrameReaderFactory {
	AudioFrameReaderFactory *factory;
public:
	SPATIALIZED_HA_SIMULATION_API explicit ChannelCopierFactory(
		AudioFrameReaderFactory *
	) noexcept;
	SPATIALIZED_HA_SIMULATION_API 
		std::shared_ptr<AudioFrameReader> make(std::string filePath) override;
};