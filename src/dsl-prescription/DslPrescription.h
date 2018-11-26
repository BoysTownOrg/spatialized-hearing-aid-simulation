#pragma once

#ifdef DSL_PRESCRIPTION_EXPORTS
	#define DSL_PRESCRIPTION_API __declspec(dllexport)
#else
	#define DSL_PRESCRIPTION_API __declspec(dllimport)
#endif

#include "DslPrescriptionFileParser.h"
#include <common-includes/RuntimeError.h>

class DslPrescription {
	// Order important for construction.
	const std::vector<double> _crossFrequenciesHz;
	const std::vector<double> _compressionRatios;
	const std::vector<double> _kneepointGains_dB;
	const std::vector<double> _kneepoints_dBSpl;
	const std::vector<double> _broadbandOutputLimitingThresholds_dBSpl;
	const int _channels;
public:
	RUNTIME_ERROR(InvalidPrescription);
	enum class Property {
		crossFrequenciesHz,
		compressionRatios,
		kneepointGains_dB,
		kneepoints_dBSpl,
		broadbandOutputLimitingThresholds_dBSpl
	};
	DSL_PRESCRIPTION_API static std::string propertyName(Property);
	DSL_PRESCRIPTION_API explicit DslPrescription(const DslPrescriptionFileParser &parser);
	DSL_PRESCRIPTION_API int channels() const;
	DSL_PRESCRIPTION_API const std::vector<double> &crossFrequenciesHz() const;
	DSL_PRESCRIPTION_API const std::vector<double> &compressionRatios() const;
	DSL_PRESCRIPTION_API const std::vector<double> &kneepointGains_dB() const;
	DSL_PRESCRIPTION_API const std::vector<double> &kneepoints_dBSpl() const;
	DSL_PRESCRIPTION_API const std::vector<double> &broadbandOutputLimitingThresholds_dBSpl() const;
};

