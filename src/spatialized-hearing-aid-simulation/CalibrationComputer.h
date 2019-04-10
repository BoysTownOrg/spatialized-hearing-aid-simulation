#pragma once

#include "AudioFrameReader.h"
#include <common-includes/Interface.h>
#include <memory>

class CalibrationComputer {
public:
    INTERFACE_OPERATIONS(CalibrationComputer)
	virtual double signalScale(int channel, double level) = 0;
};

class CalibrationComputerFactory {
public:
    INTERFACE_OPERATIONS(CalibrationComputerFactory)
	virtual std::shared_ptr<CalibrationComputer> make(AudioFrameReader *) = 0;
};
