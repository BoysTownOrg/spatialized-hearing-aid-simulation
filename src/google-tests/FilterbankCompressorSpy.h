#pragma once

#include <hearing-aid-processing/FilterbankCompressor.h>

class FilterbankCompressorSpy : public FilterbankCompressor {
	std::string processingLog_{};
	int chunkSize_ = 1;
	int compressInputChunkSize_{};
	int filterbankAnalyzeChunkSize_{};
	int compressChannelsChunkSize_{};
	int filterbankSynthesizeChunkSize_{};
	int compressOutputChunkSize_{};
	bool failed_{};
	int windowSize_{};
public:
	std::string processingLog() const {
		return processingLog_;
	}

	void compressInput(real_type *, real_type *, int chunkSize) override {
		compressInputChunkSize_ = chunkSize;
		processingLog_ += "compressInput";
	}

	void analyzeFilterbank(real_type *, complex_type *, int chunkSize) override {
		filterbankAnalyzeChunkSize_ = chunkSize;
		processingLog_ += "analyzeFilterbank";
	}

	void compressChannels(complex_type *, complex_type *, int chunkSize) override {
		compressChannelsChunkSize_ = chunkSize;
		processingLog_ += "compressChannels";
	}

	void synthesizeFilterbank(complex_type *, real_type *, int chunkSize) override {
		filterbankSynthesizeChunkSize_ = chunkSize;
		processingLog_ += "synthesizeFilterbank";
	}

	void compressOutput(real_type *, real_type *, int chunkSize) override {
		compressOutputChunkSize_ = chunkSize;
		processingLog_ += "compressOutput";
	}

	void setChunkSize(int s) {
		chunkSize_ = s;
	}

	int compressInputChunkSize() const {
		return compressInputChunkSize_;
	}

	int filterbankAnalyzeChunkSize() const {
		return filterbankAnalyzeChunkSize_;
	}

	int compressChannelsChunkSize() const {
		return compressChannelsChunkSize_;
	}

	int filterbankSynthesizeChunkSize() const {
		return filterbankSynthesizeChunkSize_;
	}

	int compressOutputChunkSize() const {
		return compressOutputChunkSize_;
	}

	int chunkSize() override {
		return chunkSize_;
	}

	int channels() override {
		return 1;
	}

	void fail() {
		failed_ = true;
	}

	bool failed() override {
		return failed_;
	}

	void setWindowSize(int n) {
		windowSize_ = n;
	}

	int windowSize() override {
		return windowSize_;
	}
};

class FilterbankCompressorSpyFactory : public FilterbankCompressorFactory {
	FilterbankCompressor::Parameters parameters_{};
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	explicit FilterbankCompressorSpyFactory(
		std::shared_ptr<FilterbankCompressor> compressor =
			std::make_shared<FilterbankCompressorSpy>()
	) :
		compressor{ std::move(compressor) } {}

	const FilterbankCompressor::Parameters &parameters() const {
		return parameters_;
	}

	std::shared_ptr<FilterbankCompressor> make(
		FilterbankCompressor::Parameters p
	) override {
		parameters_ = std::move(p);
		return compressor;
	}
};