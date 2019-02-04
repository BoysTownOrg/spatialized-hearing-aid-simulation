#pragma once

#include "AudioFrameReader.h"
#include <common-includes/Interface.h>
#include <memory>

class ICalibrationComputer {
public:
	INTERFACE_OPERATIONS(ICalibrationComputer);
	virtual double signalScale(int channel, double level) = 0;
};

class ICalibrationComputerFactory {
public:
	INTERFACE_OPERATIONS(ICalibrationComputerFactory);
	virtual std::shared_ptr<ICalibrationComputer> make(AudioFrameReader *) = 0;
};