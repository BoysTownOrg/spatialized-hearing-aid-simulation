#pragma once

#include "ArgumentCollection.h"
#include "LogString.h"
#include <hearing-aid-processing/FilterbankCompressor.h>

class FilterbankCompressorSpy : public FilterbankCompressor {
	LogString processingLog_{};
	int chunkSize_ = 1;
	int compressInputChunkSize_{};
	int filterbankAnalyzeChunkSize_{};
	int compressChannelsChunkSize_{};
	int filterbankSynthesizeChunkSize_{};
	int compressOutputChunkSize_{};
	int windowSize_{};
	bool failed_{};
public:
	auto &processingLog() const noexcept {
		return processingLog_;
	}

	void compressInput(real_type *, real_type *, int chunkSize) override {
		compressInputChunkSize_ = chunkSize;
		processingLog_.insert("compressInput");
	}

	void analyzeFilterbank(real_type *, complex_type *, int chunkSize) override {
		filterbankAnalyzeChunkSize_ = chunkSize;
		processingLog_.insert("analyzeFilterbank");
	}

	void compressChannels(complex_type *, complex_type *, int chunkSize) override {
		compressChannelsChunkSize_ = chunkSize;
		processingLog_.insert("compressChannels");
	}

	void synthesizeFilterbank(complex_type *, real_type *, int chunkSize) override {
		filterbankSynthesizeChunkSize_ = chunkSize;
		processingLog_.insert("synthesizeFilterbank");
	}

	void compressOutput(real_type *, real_type *, int chunkSize) override {
		compressOutputChunkSize_ = chunkSize;
		processingLog_.insert("compressOutput");
	}

	void setChunkSize(int s) noexcept {
		chunkSize_ = s;
	}

	auto compressInputChunkSize() const noexcept {
		return compressInputChunkSize_;
	}

	auto filterbankAnalyzeChunkSize() const noexcept {
		return filterbankAnalyzeChunkSize_;
	}

	auto compressChannelsChunkSize() const noexcept {
		return compressChannelsChunkSize_;
	}

	auto filterbankSynthesizeChunkSize() const noexcept {
		return filterbankSynthesizeChunkSize_;
	}

	auto compressOutputChunkSize() const noexcept {
		return compressOutputChunkSize_;
	}

	int chunkSize() override {
		return chunkSize_;
	}

	int channels() override {
		return 1;
	}

	void fail() noexcept {
		failed_ = true;
	}

	bool failed() override {
		return failed_;
	}

	void setWindowSize(int n) noexcept {
		windowSize_ = n;
	}

	int windowSize() override {
		return windowSize_;
	}
};

class FilterbankCompressorSpyFactory : public FilterbankCompressorFactory {
	ArgumentCollection<FilterbankCompressor::Parameters> parameters_{};
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	explicit FilterbankCompressorSpyFactory(
		std::shared_ptr<FilterbankCompressor> compressor =
			std::make_shared<FilterbankCompressorSpy>()
	) noexcept :
		compressor{ std::move(compressor) } {}

	auto parameters() const {
		return parameters_;
	}

	std::shared_ptr<FilterbankCompressor> make(
		FilterbankCompressor::Parameters p
	) override {
		parameters_.push_back(std::move(p));
		return compressor;
	}
};