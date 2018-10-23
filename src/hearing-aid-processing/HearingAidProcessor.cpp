#include "HearingAidProcessor.h"

HearingAidProcessor::HearingAidProcessor(
	std::shared_ptr<FilterbankCompressor> compressor
) :
	compressor{ std::move(compressor) } {}

void HearingAidProcessor::process(int frameCount) {
	const auto chunkSize = compressor->chunkSize();
	if (frameCount == chunkSize) {
		compressor->compressInput(nullptr, nullptr, chunkSize);
		compressor->analyzeFilterbank(nullptr, nullptr, chunkSize);
		compressor->compressChannels(nullptr, nullptr, chunkSize);
		compressor->synthesizeFilterbank(nullptr, nullptr, chunkSize);
		compressor->compressOutput(nullptr, nullptr, chunkSize);
	}
}
