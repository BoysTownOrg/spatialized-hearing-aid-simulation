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
	const double _attack_ms;
	const double _release_ms;
	const int _channels;
	const int _chunkSize;
	const int _windowSize;
public:
	class InvalidPrescription : public std::runtime_error {
	public:
		explicit InvalidPrescription(std::string what) : std::runtime_error{ what } {}
	};
	DSL_PRESCRIPTION_API explicit DslPrescription(const DslPrescriptionFileParser &parser);
	DSL_PRESCRIPTION_API int channels() const;
	DSL_PRESCRIPTION_API int chunkSize() const;
	DSL_PRESCRIPTION_API int windowSize() const;
	DSL_PRESCRIPTION_API double attack_ms() const;
	DSL_PRESCRIPTION_API double release_ms() const;
	DSL_PRESCRIPTION_API const std::vector<double> &crossFrequenciesHz() const;
	DSL_PRESCRIPTION_API const std::vector<double> &compressionRatios() const;
	DSL_PRESCRIPTION_API const std::vector<double> &kneepointGains_dB() const;
	DSL_PRESCRIPTION_API const std::vector<double> &kneepoints_dBSpl() const;
	DSL_PRESCRIPTION_API const std::vector<double> &broadbandOutputLimitingThresholds_dBSpl() const;
};

