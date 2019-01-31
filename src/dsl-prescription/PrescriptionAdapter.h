#pragma once

#include "ConfigurationFileParser.h"
#include "PrescriptionReader.h"
#include <string>

#ifdef DSL_PRESCRIPTION_EXPORTS
	#define DSL_PRESCRIPTION_API __declspec(dllexport)
#else
	#define DSL_PRESCRIPTION_API __declspec(dllimport)
#endif

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

class PrescriptionAdapter : public PrescriptionReader {
	ConfigurationFileParserFactory *factory;
public:
	DSL_PRESCRIPTION_API explicit PrescriptionAdapter(
		ConfigurationFileParserFactory *
	);
	DSL_PRESCRIPTION_API Dsl read(std::string filePath) override;
private:
	Dsl read_(std::string filePath);
};

