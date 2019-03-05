#pragma once

#include "ArgumentCollection.h"
#include <spatialized-hearing-aid-simulation/CalibrationComputerImpl.h>
#include <map>

class CalibrationComputerStub : public CalibrationComputer {
	ArgumentCollection<double> levels_{};
	std::map<int, double> signalScales;
public:
	double signalScale(int channel, double level) override {
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

class CalibrationComputerStubFactory : public CalibrationComputerFactory {
	std::shared_ptr<CalibrationComputer> computer;
	AudioFrameReader *reader_;
public:
	explicit CalibrationComputerStubFactory(
		std::shared_ptr<CalibrationComputer> computer =
			std::make_shared<CalibrationComputerStub>()
	) :
		computer{ computer } {}

	std::shared_ptr<CalibrationComputer> make(AudioFrameReader *r) override {
		reader_ = r;
		return computer;
	}

	auto reader() const noexcept {
		return reader_;
	}
};