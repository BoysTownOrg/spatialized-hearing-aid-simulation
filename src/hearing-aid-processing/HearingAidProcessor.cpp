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

void HearingAidProcessor::process(gsl::span<float> signal) {
	const auto chunkSize = compressor->chunkSize();
	if (signal.size() == chunkSize) {
		const auto complex = &complexBuffer.at(0);
		compressor->compressInput(signal.data(), signal.data(), chunkSize);
		compressor->analyzeFilterbank(signal.data(), complex, chunkSize);
		compressor->compressChannels(complex, complex, chunkSize);
		compressor->synthesizeFilterbank(complex, signal.data(), chunkSize);
		compressor->compressOutput(signal.data(), signal.data(), chunkSize);
	}
}
