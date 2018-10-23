#include "HearingAidProcessor.h"

HearingAidProcessor::HearingAidProcessor(
	std::shared_ptr<FilterbankCompressor> compressor
) :
	compressor{ std::move(compressor) } {}

void HearingAidProcessor::process(float *x, int frameCount) {
	const auto chunkSize = compressor->chunkSize();
	if (frameCount == chunkSize) {
		compressor->compressInput(x, x, chunkSize);
		compressor->analyzeFilterbank(x, nullptr, chunkSize);
		compressor->compressChannels(nullptr, nullptr, chunkSize);
		compressor->synthesizeFilterbank(nullptr, x, chunkSize);
		compressor->compressOutput(x, x, chunkSize);
	}
}
