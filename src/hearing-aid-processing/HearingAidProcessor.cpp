#include "HearingAidProcessor.h"

HearingAidProcessor::HearingAidProcessor(
	std::shared_ptr<FilterbankCompressor> compressor
) :
	complexBuffer(compressor->channels() * compressor->chunkSize() * 2),
	compressor{ std::move(compressor) } {}

void HearingAidProcessor::process(float *x, int frameCount) {
	const auto chunkSize = compressor->chunkSize();
	if (frameCount == chunkSize) {
		const auto complex = &complexBuffer[0];
		compressor->compressInput(x, x, chunkSize);
		compressor->analyzeFilterbank(x, complex, chunkSize);
		compressor->compressChannels(complex, complex, chunkSize);
		compressor->synthesizeFilterbank(complex, x, chunkSize);
		compressor->compressOutput(x, x, chunkSize);
	}
}
