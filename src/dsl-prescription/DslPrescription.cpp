#include "DslPrescription.h"
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

DslPrescription::DslPrescription(const ConfigurationFileParser &parser) 
	try :
	_crossFrequenciesHz(parser.asVector(
		propertyName(dsl_prescription::Property::crossFrequenciesHz))),
	_compressionRatios(parser.asVector(
		propertyName(dsl_prescription::Property::compressionRatios))),
	_kneepointGains_dB(parser.asVector(
		propertyName(dsl_prescription::Property::kneepointGains_dB))),
	_kneepoints_dBSpl(parser.asVector(
		propertyName(dsl_prescription::Property::kneepoints_dBSpl))),
	_broadbandOutputLimitingThresholds_dBSpl(
		parser.asVector(propertyName(
			dsl_prescription::Property::broadbandOutputLimitingThresholds_dBSpl))),
	_channels(gsl::narrow<int>(_crossFrequenciesHz.size() + 1))
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
catch (const ConfigurationFileParser::ParseError &e) {
	throw InvalidPrescription{ e.what() };
}

const std::vector<double> &
	DslPrescription::broadbandOutputLimitingThresholds_dBSpl() const 
{
	return _broadbandOutputLimitingThresholds_dBSpl;
}

const std::vector<double> &DslPrescription::kneepoints_dBSpl() const {
	return _kneepoints_dBSpl;
}

const std::vector<double> &DslPrescription::kneepointGains_dB() const {
	return _kneepointGains_dB;
}

const std::vector<double> &DslPrescription::compressionRatios() const {
	return _compressionRatios;
}

const std::vector<double> &DslPrescription::crossFrequenciesHz() const {
	return _crossFrequenciesHz;
}

int DslPrescription::channels() const {
	return _channels;
}
