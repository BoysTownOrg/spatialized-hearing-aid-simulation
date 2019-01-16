#include "HearingAidProcessor.h"

static bool powerOfTwo(int n) {
	return n > 0 && (n & (n - 1)) == 0;
}

HearingAidProcessor::HearingAidProcessor(
	std::shared_ptr<FilterbankCompressor> compressor
) :
	buffer(compressor->channels() * compressor->chunkSize() * 2),
	compressor{ std::move(compressor) } 
{
	if (this->compressor->failed())
		throw CompressorError{ "The compressor failed to initialize." };
	if (!powerOfTwo(this->compressor->chunkSize()))
		throw CompressorError{ "The chunk size must be a power of two." };
}

void HearingAidProcessor::process(gsl::span<float> signal) {
	const auto chunkSize = compressor->chunkSize();
	if (signal.size() == chunkSize) {
		const auto buffer_ = &buffer.front();
		compressor->compressInput(signal.data(), signal.data(), chunkSize);
		compressor->analyzeFilterbank(signal.data(), buffer_, chunkSize);
		compressor->compressChannels(buffer_, buffer_, chunkSize);
		compressor->synthesizeFilterbank(buffer_, signal.data(), chunkSize);
		compressor->compressOutput(signal.data(), signal.data(), chunkSize);
	}
}

int HearingAidProcessor::groupDelay() {
	return compressor->windowSize() / 2;
}
