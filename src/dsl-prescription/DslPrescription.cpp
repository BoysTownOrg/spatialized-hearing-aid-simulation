#include "DslPrescription.h"

DslPrescription::DslPrescription(const DslPrescriptionFileParser &parser) 
	try :
	_chunkSize(parser.asInt("chunk_size")),
	_windowSize(parser.asInt("window_size")),
	_attack_ms(parser.asDouble("attack_ms")),
	_release_ms(parser.asDouble("release_ms")),
	_crossFrequenciesHz(parser.asVector("cross_frequencies_Hz")),
	_compressionRatios(parser.asVector("compression_ratios")),
	_kneepointGains_dB(parser.asVector("kneepoint_gains_dB")),
	_kneepoints_dBSpl(parser.asVector("kneepoints_dB_SPL")),
	_broadbandOutputLimitingThresholds_dBSpl(
		parser.asVector("BOLT_dB_SPL")),
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