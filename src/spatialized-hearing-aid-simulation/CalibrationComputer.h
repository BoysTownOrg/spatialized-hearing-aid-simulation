#pragma once

#include "AudioFrameReader.h"
#include "spatialized-hearing-aid-simulation-exports.h"

class CalibrationComputer {
	using sample_type = AudioFrameReader::channel_type::element_type;
	using channel_type = std::vector<sample_type>;
    std::vector<channel_type> audioFileContents;
public:
	SPATIALIZED_HA_SIMULATION_API explicit CalibrationComputer(AudioFrameReader &reader);
	SPATIALIZED_HA_SIMULATION_API double signalScale(int channel, double level);

private:
	void read(AudioFrameReader & reader);
};

