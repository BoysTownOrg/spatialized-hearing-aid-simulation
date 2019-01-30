#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>

class PrescriptionReader {
public:
	INTERFACE_OPERATIONS(PrescriptionReader);
	struct Dsl {
		std::vector<double> crossFrequenciesHz;
		std::vector<double> compressionRatios;
		std::vector<double> kneepointGains_dB;
		std::vector<double> kneepoints_dBSpl;
		std::vector<double> broadbandOutputLimitingThresholds_dBSpl;
		int channels;
	};
	virtual Dsl read(std::string filePath) = 0;
	RUNTIME_ERROR(ReadFailure);
};