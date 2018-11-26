#include "Chapro.h"
extern "C" {
#include <cha_ff.h>
}

Chapro::Chapro(
	const DslPrescription &prescription,
	Parameters parameters
) :
	_chunkSize(parameters.chunkSize),
	_channels(prescription.channels())
{
	CHA_DSL dsl;
	dsl.attack = parameters.attack_ms;
	dsl.release = parameters.release_ms;
	dsl.nchannel = _channels;
	using size_type = std::vector<double>::size_type;
	for (size_type i = 0; i < prescription.crossFrequenciesHz().size(); ++i)
		dsl.cross_freq[i] = prescription.crossFrequenciesHz()[i];
	for (size_type i = 0; i < prescription.compressionRatios().size(); ++i)
		dsl.cr[i] = prescription.compressionRatios()[i];
	for (size_type i = 0; i < prescription.kneepoints_dBSpl().size(); ++i)
		dsl.tk[i] = prescription.kneepoints_dBSpl()[i];
	for (size_type i = 0; i < prescription.kneepointGains_dB().size(); ++i)
		dsl.tkgain[i] = prescription.kneepointGains_dB()[i];
	for (
		size_type i = 0;
		i < prescription.broadbandOutputLimitingThresholds_dBSpl().size();
		++i
	)
		dsl.bolt[i] = prescription.broadbandOutputLimitingThresholds_dBSpl()[i];
	CHA_WDRC wdrc;
	wdrc.attack = 1;
	wdrc.release = 50;
	wdrc.fs = parameters.sampleRate;
	wdrc.maxdB = 119;
	wdrc.tkgain = 0;
	wdrc.tk = 105;
	wdrc.cr = 10;
	wdrc.bolt = 105;
	const auto hamming = 0;
	cha_firfb_prepare(
		cha_pointer,
		dsl.cross_freq,
		_channels,
		parameters.sampleRate,
		parameters.windowSize,
		hamming,
		_chunkSize);
	cha_agc_prepare(cha_pointer, &dsl, &wdrc);
}

Chapro::~Chapro() noexcept {
	cha_cleanup(cha_pointer);
}

void Chapro::compressInput(float *input, float *output, int chunkSize) {
	cha_agc_input(cha_pointer, input, output, chunkSize);
}

void Chapro::analyzeFilterbank(float *input, float *output, int chunkSize) {
	cha_firfb_analyze(cha_pointer, input, output, chunkSize);
}

void Chapro::compressChannels(float *input, float *output, int chunkSize) {
	cha_agc_channel(cha_pointer, input, output, chunkSize);
}

void Chapro::synthesizeFilterbank(float *input, float *output, int chunkSize) {
	cha_firfb_synthesize(cha_pointer, input, output, chunkSize);
}

void Chapro::compressOutput(float *input, float *output, int chunkSize) {
	cha_agc_output(cha_pointer, input, output, chunkSize);
}

int Chapro::chunkSize() const {
	return _chunkSize;
}

int Chapro::channels() const {
	return _channels;
}
