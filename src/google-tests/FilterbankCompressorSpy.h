#pragma once

#include <hearing-aid-processing/FilterbankCompressor.h>

class FilterbankCompressorSpy : public FilterbankCompressor {
	std::string _processingLog{};
	int _chunkSize = 1;
	int _compressInputChunkSize{};
	int _filterbankAnalyzeChunkSize{};
	int _compressChannelsChunkSize{};
	int _filterbankSynthesizeChunkSize{};
	int _compressOutputChunkSize{};
	bool _failed{};
	int windowSize_{};
public:
	std::string processingLog() const {
		return _processingLog;
	}

	void compressInput(real_type *, real_type *, int chunkSize) override {
		_compressInputChunkSize = chunkSize;
		_processingLog += "compressInput";
	}

	void analyzeFilterbank(real_type *, complex_type *, int chunkSize) override {
		_filterbankAnalyzeChunkSize = chunkSize;
		_processingLog += "analyzeFilterbank";
	}

	void compressChannels(complex_type *, complex_type *, int chunkSize) override {
		_compressChannelsChunkSize = chunkSize;
		_processingLog += "compressChannels";
	}

	void synthesizeFilterbank(complex_type *, real_type *, int chunkSize) override {
		_filterbankSynthesizeChunkSize = chunkSize;
		_processingLog += "synthesizeFilterbank";
	}

	void compressOutput(real_type *, real_type *, int chunkSize) override {
		_compressOutputChunkSize = chunkSize;
		_processingLog += "compressOutput";
	}

	void setChunkSize(int s) {
		_chunkSize = s;
	}

	int compressInputChunkSize() const {
		return _compressInputChunkSize;
	}

	int filterbankAnalyzeChunkSize() const {
		return _filterbankAnalyzeChunkSize;
	}

	int compressChannelsChunkSize() const {
		return _compressChannelsChunkSize;
	}

	int filterbankSynthesizeChunkSize() const {
		return _filterbankSynthesizeChunkSize;
	}

	int compressOutputChunkSize() const {
		return _compressOutputChunkSize;
	}

	int chunkSize() const override {
		return _chunkSize;
	}

	int channels() const override {
		return 1;
	}

	void fail() {
		_failed = true;
	}

	bool failed() const override {
		return _failed;
	}

	void setWindowSize(int n) {
		windowSize_ = n;
	}

	int windowSize() const override {
		return windowSize_;
	}
};

class MockCompressorFactory : public FilterbankCompressorFactory {
	FilterbankCompressor::Parameters _parameters{};
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	explicit MockCompressorFactory(
		std::shared_ptr<FilterbankCompressor> compressor =
		std::make_shared<FilterbankCompressorSpy>()
	) :
		compressor{ std::move(compressor) } {}

	const FilterbankCompressor::Parameters &parameters() const {
		return _parameters;
	}

	std::shared_ptr<FilterbankCompressor> make(
		FilterbankCompressor::Parameters p
	) override {
		_parameters = p;
		return compressor;
	}
};