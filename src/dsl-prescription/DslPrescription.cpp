#include "DslPrescription.h"

DslPrescription::DslPrescription(const DslPrescriptionFileParser &parser) :
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