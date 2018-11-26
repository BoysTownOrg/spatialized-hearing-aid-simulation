#pragma once

#ifdef DSL_PRESCRIPTION_EXPORTS
	#define DSL_PRESCRIPTION_API __declspec(dllexport)
#else
	#define DSL_PRESCRIPTION_API __declspec(dllimport)
#endif

#include "DslPrescriptionFileParser.h"
#include <string>
#include <stdexcept>

class DslPrescription {
	// Order important for construction.
	const std::vector<double> _crossFrequenciesHz;
	const std::vector<double> _compressionRatios;
	const std::vector<double> _kneepointGains_dB;
	const std::vector<double> _kneepoints_dBSpl;
	const std::vector<double> _broadbandOutputLimitingThresholds_dBSpl;
public:
	class InvalidPrescription : public std::runtime_error {
	public:
		explicit InvalidPrescription(std::string what) : std::runtime_error{ what } {}
	};
	DSL_PRESCRIPTION_API explicit DslPrescription(const DslPrescriptionFileParser &parser);
};

