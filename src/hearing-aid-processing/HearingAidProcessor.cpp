#include "HearingAidProcessor.h"

HearingAidProcessor::HearingAidProcessor(
	std::shared_ptr<FilterbankCompressor> compressor
) :
	buffer(compressor->channels() * compressor->chunkSize() * 2),
	compressor{ std::move(compressor) } 
{
	if (this->compressor->failed())
		throw CompressorError{ "The compressor failed to initialize." };
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

int HearingAidProcessor::groupDelay()
{
	return 0;
}
