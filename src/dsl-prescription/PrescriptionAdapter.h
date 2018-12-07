#pragma once

#ifdef DSL_PRESCRIPTION_EXPORTS
	#define DSL_PRESCRIPTION_API __declspec(dllexport)
#else
	#define DSL_PRESCRIPTION_API __declspec(dllimport)
#endif

#include <string>

namespace dsl_prescription {
	enum class Property {
		crossFrequenciesHz,
		compressionRatios,
		kneepointGains_dB,
		kneepoints_dBSpl,
		broadbandOutputLimitingThresholds_dBSpl
	};

	DSL_PRESCRIPTION_API std::string propertyName(Property);
}

#include "ConfigurationFileParser.h"
#include "PrescriptionReader.h"

class PrescriptionAdapter : public PrescriptionReader {
	std::shared_ptr<ConfigurationFileParserFactory> factory;
public:
	DSL_PRESCRIPTION_API explicit PrescriptionAdapter(
		std::shared_ptr<ConfigurationFileParserFactory> factory);
	DSL_PRESCRIPTION_API Dsl read(std::string filePath) override;
};

