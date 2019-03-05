#pragma once

#include "CalibrationComputer.h"
#include "spatialized-hearing-aid-simulation-exports.h"

class CalibrationComputerImpl : public CalibrationComputer {
	using sample_type = AudioFrameReader::channel_type::element_type;
	using channel_type = std::vector<sample_type>;
    std::vector<channel_type> audioFileContents;
public:
	SPATIALIZED_HA_SIMULATION_API explicit CalibrationComputerImpl(AudioFrameReader &reader);
	SPATIALIZED_HA_SIMULATION_API double signalScale(int channel, double level) override;

private:
	bool validChannel(int channel);
	void read(AudioFrameReader & reader);
};

