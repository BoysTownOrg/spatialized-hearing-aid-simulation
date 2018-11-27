#include "HearingAidProcessor.h"

HearingAidProcessor::HearingAidProcessor(
	std::shared_ptr<FilterbankCompressor> compressor
) :
	complexBuffer(compressor->channels() * compressor->chunkSize() * 2),
	compressor{ std::move(compressor) } 
{
	if (this->compressor->failed())
		throw CompressorError{ "The compressor failed to initialize." };
}

void HearingAidProcessor::process(float *signal, int samples) {
	const auto chunkSize = compressor->chunkSize();
	if (samples == chunkSize) {
		const auto complex = &complexBuffer[0];
		compressor->compressInput(signal, signal, chunkSize);
		compressor->analyzeFilterbank(signal, complex, chunkSize);
		compressor->compressChannels(complex, complex, chunkSize);
		compressor->synthesizeFilterbank(complex, signal, chunkSize);
		compressor->compressOutput(signal, signal, chunkSize);
	}
}
