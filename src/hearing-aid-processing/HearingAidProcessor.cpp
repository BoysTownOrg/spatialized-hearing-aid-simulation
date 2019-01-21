#include "HearingAidProcessor.h"

HearingAidProcessor::HearingAidProcessor(
	std::shared_ptr<FilterbankCompressor> compressor
) :
	buffer(compressor->channels() * compressor->chunkSize() * 2),
	compressor{ std::move(compressor) } 
{
	if (this->compressor->failed())
		throw CompressorError{ "The compressor failed to initialize." };
	throwIfNotPowerOfTwo(this->compressor->chunkSize(), "chunk size");
	throwIfNotPowerOfTwo(this->compressor->windowSize(), "window size");
}

static bool powerOfTwo(int n) {
	return n > 0 && (n & (n - 1)) == 0;
}

void HearingAidProcessor::throwIfNotPowerOfTwo(int n, std::string name) {
	if (!powerOfTwo(n))
		throw CompressorError{ "The " + name + " must be a power of two." };
}

void HearingAidProcessor::process(signal_type signal) {
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

auto HearingAidProcessor::groupDelay() -> index_type {
	return compressor->windowSize() / 2;
}
