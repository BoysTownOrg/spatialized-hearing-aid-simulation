#include "PrescriptionAdapter.h"
#include <gsl/gsl>

std::string dsl_prescription::propertyName(Property p) {
	switch (p) {
	case Property::crossFrequenciesHz:
		return "cross_frequencies_Hz";
	case Property::compressionRatios:
		return "compression_ratios";
	case Property::kneepointGains_dB:
		return "kneepoint_gains_dB";
	case Property::kneepoints_dBSpl:
		return "kneepoints_dB_SPL";
	case Property::broadbandOutputLimitingThresholds_dBSpl:
		return "BOLT_dB_SPL";
	default:
		return "unknown";
	}
}

PrescriptionAdapter::PrescriptionAdapter(std::shared_ptr<ConfigurationFileParserFactory> factory) :
	factory{ std::move(factory) }
{
}

auto PrescriptionAdapter::read(std::string filePath) -> Dsl {
	try {
		const auto parser = factory->make(filePath);
		Dsl dsl{};
		dsl.crossFrequenciesHz = parser->asVector(propertyName(dsl_prescription::Property::crossFrequenciesHz));
		dsl.compressionRatios = parser->asVector(propertyName(dsl_prescription::Property::compressionRatios));
		dsl.kneepointGains_dB = parser->asVector(propertyName(dsl_prescription::Property::kneepointGains_dB));
		dsl.kneepoints_dBSpl = parser->asVector(propertyName(dsl_prescription::Property::kneepoints_dBSpl));
		dsl.broadbandOutputLimitingThresholds_dBSpl = parser->asVector(propertyName(dsl_prescription::Property::broadbandOutputLimitingThresholds_dBSpl));
		dsl.channels = gsl::narrow<int>(dsl.crossFrequenciesHz.size() + 1);
		const auto channels = dsl.crossFrequenciesHz.size() + 1;
		if (
			dsl.compressionRatios.size() != channels ||
			dsl.kneepointGains_dB.size() != channels ||
			dsl.kneepoints_dBSpl.size() != channels ||
			dsl.broadbandOutputLimitingThresholds_dBSpl.size() != channels
		)
			throw ReadError{ "channel count mismatch in prescription." };
		return dsl;
	}
	catch (const ConfigurationFileParser::ParseError &e) {
		throw ReadError{ e.what() };
	}
}
