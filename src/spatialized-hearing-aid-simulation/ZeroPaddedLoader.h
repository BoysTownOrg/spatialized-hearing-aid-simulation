#pragma once

#include "AudioProcessingLoader.h"
#include "spatialized-hearing-aid-simulation-exports.h"

class ZeroPaddedLoader : public AudioLoader {
	std::shared_ptr<AudioFrameProcessor> processor;
	std::shared_ptr<AudioFrameReader> reader;
	long long paddedZeros{};
public:
	SPATIALIZED_HA_SIMULATION_API ZeroPaddedLoader(
		std::shared_ptr<AudioFrameReader> reader,
		std::shared_ptr<AudioFrameProcessor> processor
	) noexcept;
	SPATIALIZED_HA_SIMULATION_API void reset();
	SPATIALIZED_HA_SIMULATION_API void load(gsl::span<channel_type> audio) override;
	SPATIALIZED_HA_SIMULATION_API bool complete() override;
private:
	void padZeros(gsl::span<channel_type> audio, long long zerosToPad);
};

class ZeroPaddedLoaderFactory : public AudioProcessingLoaderFactory {
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<AudioLoader> make(
		std::shared_ptr<AudioFrameReader>, 
		std::shared_ptr<AudioFrameProcessor>
	) override;
};