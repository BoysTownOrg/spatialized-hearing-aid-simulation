#include "Chapro.h"
extern "C" {
#include <cha_ff.h>
}

Chapro::Chapro(Parameters parameters) :
	chunkSize_{ parameters.chunkSize },
	channels_{ parameters.channels },
	windowSize_{ parameters.windowSize }
{
	CHA_DSL dsl{};
	dsl.attack = parameters.attack_ms;
	dsl.release = parameters.release_ms;
	dsl.nchannel = channels_;
	using size_type = std::vector<double>::size_type;
	for (size_type i = 0; i < parameters.crossFrequenciesHz.size(); ++i)
		dsl.cross_freq[i] = parameters.crossFrequenciesHz.at(i);
	for (size_type i = 0; i < parameters.compressionRatios.size(); ++i)
		dsl.cr[i] = parameters.compressionRatios.at(i);
	for (size_type i = 0; i < parameters.kneepoints_dBSpl.size(); ++i)
		dsl.tk[i] = parameters.kneepoints_dBSpl.at(i);
	for (size_type i = 0; i < parameters.kneepointGains_dB.size(); ++i)
		dsl.tkgain[i] = parameters.kneepointGains_dB.at(i);
	for (
		size_type i = 0;
		i < parameters.broadbandOutputLimitingThresholds_dBSpl.size();
		++i
	)
		dsl.bolt[i] = parameters.broadbandOutputLimitingThresholds_dBSpl.at(i);
	CHA_WDRC wdrc;
	wdrc.attack = 1;
	wdrc.release = 50;
	wdrc.fs = parameters.sampleRate;
	wdrc.maxdB = parameters.max_dB_Spl;
	wdrc.tkgain = 0;
	wdrc.tk = 105;
	wdrc.cr = 10;
	wdrc.bolt = 105;
	const auto hamming = 0;
	error = cha_firfb_prepare(
		cha_pointer,
		dsl.cross_freq,
		channels_,
		parameters.sampleRate,
		windowSize_,
		hamming,
		chunkSize_
	);
	error |= cha_agc_prepare(cha_pointer, &dsl, &wdrc);
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

int Chapro::chunkSize() {
	return chunkSize_;
}

int Chapro::channels() {
	return channels_;
}

bool Chapro::failed() {
	return error != 0;
}

int Chapro::windowSize() {
	return windowSize_;
}
