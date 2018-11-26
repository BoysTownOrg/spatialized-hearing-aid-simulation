#pragma once

#include <common-includes/Interface.h>
#include <stdexcept>
#include <vector>
#include <string>

class DslPrescriptionFileParser {
public:
	INTERFACE_OPERATIONS(DslPrescriptionFileParser);
	class ParseError : public std::runtime_error {
	public:
		explicit ParseError(std::string what) : std::runtime_error{ what } {}
	};
	virtual std::vector<double> asVector(std::string property) const = 0;
};

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
	explicit DslPrescription(const DslPrescriptionFileParser &parser) :
		_compressionRatios(parser.asVector("compression_ratios")),
		_kneepointGains_dB(parser.asVector("kneepoint_gains_dB")),
		_kneepoints_dBSpl(parser.asVector("kneepoints_dB_SPL")),
		_broadbandOutputLimitingThresholds_dBSpl(
			parser.asVector("BOLT_dB_SPL"))
	{
		const auto channels = _crossFrequenciesHz.size() + 1;
		if (
			_compressionRatios.size() != channels ||
			_kneepointGains_dB.size() != channels ||
			_kneepoints_dBSpl.size() != channels ||
			_broadbandOutputLimitingThresholds_dBSpl.size() != channels
			)
			throw InvalidPrescription{ "channel count mismatch in prescription." };
	}
};

