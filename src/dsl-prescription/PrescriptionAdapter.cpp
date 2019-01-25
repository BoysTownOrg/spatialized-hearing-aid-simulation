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
		return read_(std::move(filePath));
	}
	catch (const ConfigurationFileParser::ParseError &e) {
		throw ReadFailure{ e.what() };
	}
}

class VectorParser {
	std::shared_ptr<ConfigurationFileParser> parser;
public:
	explicit VectorParser(std::shared_ptr<ConfigurationFileParser> parser) :
		parser{ std::move(parser) } {}

	std::vector<double> parse(dsl_prescription::Property p) {
		return parser->asVector(dsl_prescription::propertyName(p));
	}
};

auto PrescriptionAdapter::read_(std::string filePath) -> Dsl {
	VectorParser parser{factory->make(std::move(filePath))};
	Dsl dsl{};
	using namespace dsl_prescription;
	dsl.crossFrequenciesHz = parser.parse(Property::crossFrequenciesHz);
	dsl.compressionRatios = parser.parse(Property::compressionRatios);
	dsl.kneepointGains_dB = parser.parse(Property::kneepointGains_dB);
	dsl.kneepoints_dBSpl = parser.parse(Property::kneepoints_dBSpl);
	dsl.broadbandOutputLimitingThresholds_dBSpl =
		parser.parse(Property::broadbandOutputLimitingThresholds_dBSpl);
	auto channels = dsl.crossFrequenciesHz.size() + 1;
	if (
		dsl.compressionRatios.size() != channels ||
		dsl.kneepointGains_dB.size() != channels ||
		dsl.kneepoints_dBSpl.size() != channels ||
		dsl.broadbandOutputLimitingThresholds_dBSpl.size() != channels
	)
		throw ReadFailure{ "channel count mismatch in prescription." };
	dsl.channels = gsl::narrow<int>(channels);
	return dsl;
}
