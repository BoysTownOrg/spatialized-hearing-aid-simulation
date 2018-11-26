#include "DslPrescription.h"

std::string DslPrescription::propertyName(Property p) {
	if (p == Property::chunkSize)
		return "chunk_size";
	else if (p == Property::windowSize)
		return "window_size";
	else if (p == Property::attack_ms)
		return "attack_ms";
	else if (p == Property::release_ms)
		return "release_ms";
	else if (p == Property::crossFrequenciesHz)
		return "cross_frequencies_Hz";
	else if (p == Property::compressionRatios)
		return "compression_ratios";
	else if (p == Property::kneepointGains_dB)
		return "kneepoint_gains_dB";
	else if (p == Property::kneepoints_dBSpl)
		return "kneepoints_dB_SPL";
	else if (p == Property::broadbandOutputLimitingThresholds_dBSpl)
		return "BOLT_dB_SPL";
	else
		return "unknown";
}

DslPrescription::DslPrescription(const DslPrescriptionFileParser &parser) 
	try :
	_chunkSize(parser.asInt(propertyName(Property::chunkSize))),
	_windowSize(parser.asInt(propertyName(Property::windowSize))),
	_attack_ms(parser.asDouble(propertyName(Property::attack_ms))),
	_release_ms(parser.asDouble(propertyName(Property::release_ms))),
	_crossFrequenciesHz(parser.asVector(propertyName(Property::crossFrequenciesHz))),
	_compressionRatios(parser.asVector(propertyName(Property::compressionRatios))),
	_kneepointGains_dB(parser.asVector(propertyName(Property::kneepointGains_dB))),
	_kneepoints_dBSpl(parser.asVector(propertyName(Property::kneepoints_dBSpl))),
	_broadbandOutputLimitingThresholds_dBSpl(
		parser.asVector(propertyName(Property::broadbandOutputLimitingThresholds_dBSpl))),
	_channels(static_cast<int>(_crossFrequenciesHz.size() + 1))
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
catch (const DslPrescriptionFileParser::ParseError &e) {
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

double DslPrescription::release_ms() const {
	return _release_ms;
}

double DslPrescription::attack_ms() const {
	return _attack_ms;
}

int DslPrescription::channels() const {
	return _channels;
}

int DslPrescription::chunkSize() const {
	return _chunkSize;
}

int DslPrescription::windowSize() const {
	return _windowSize;
}