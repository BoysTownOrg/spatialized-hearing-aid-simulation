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

PrescriptionAdapter::PrescriptionAdapter(ConfigurationFileParserFactory *factory) noexcept :
	factory{ factory } {}

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
	explicit VectorParser(std::shared_ptr<ConfigurationFileParser> parser) noexcept :
		parser{ std::move(parser) } {}

	std::vector<double> parse(dsl_prescription::Property p) {
		return parser->asVector(propertyName(p));
	}
};

template<typename T>
static bool allEqualTo(T expected, std::vector<T> toCheck) noexcept {
	for (auto item : toCheck)
		if (item != expected)
			return false;
	return true;
}

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
	const auto channels = dsl.crossFrequenciesHz.size() + 1;
	if (!allEqualTo(
		channels, 
		{
			dsl.compressionRatios.size(), 
			dsl.kneepointGains_dB.size(), 
			dsl.kneepoints_dBSpl.size(), 
			dsl.broadbandOutputLimitingThresholds_dBSpl.size()
		}
	))
		throw ReadFailure{ "channel count mismatch in prescription." };
	dsl.channels = gsl::narrow<int>(channels);
	return dsl;
}
