#pragma once

#include "ArgumentCollection.h"
#include <spatialized-hearing-aid-simulation/CalibrationComputer.h>
#include <map>

class CalibrationComputerStub : public ICalibrationComputer {
	ArgumentCollection<double> levels_{};
	std::map<int, double> signalScales;
public:
	double signalScale(int channel, double level) override
	{
		levels_.push_back(level);
		return signalScales[channel];
	}

	void addSignalScale(int channel, double scale) {
		signalScales[channel] = scale;
	}

	auto levels() const {
		return levels_;
	}
};

class CalibrationComputerStubFactory : public ICalibrationComputerFactory {
	std::shared_ptr<ICalibrationComputer> computer;
	AudioFrameReader *reader_;
public:
	explicit CalibrationComputerStubFactory(
		std::shared_ptr<ICalibrationComputer> computer =
			std::make_shared<CalibrationComputerStub>()
	) :
		computer{ computer } {}

	std::shared_ptr<ICalibrationComputer> make(AudioFrameReader *r) override
	{
		reader_ = r;
		return computer;
	}

	auto reader() const {
		return reader_;
	}
};