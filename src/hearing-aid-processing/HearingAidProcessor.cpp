#include "HearingAidProcessor.h"

HearingAidProcessor::HearingAidProcessor(
	std::shared_ptr<FilterbankCompressor> compressor
) :
	compressor{ std::move(compressor) } {}

void HearingAidProcessor::process() {
	compressor->compressInput(nullptr, nullptr, 0);
	compressor->analyzeFilterbank(nullptr, nullptr, 0);
	compressor->compressChannels(nullptr, nullptr, 0);
	compressor->synthesizeFilterbank(nullptr, nullptr, 0);
	compressor->compressOutput(nullptr, nullptr, 0);
}
