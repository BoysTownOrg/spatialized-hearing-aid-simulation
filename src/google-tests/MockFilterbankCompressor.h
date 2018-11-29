#pragma once

#include <hearing-aid-processing/FilterbankCompressor.h>

class MockFilterbankCompressor : public FilterbankCompressor {
	std::string _processingLog{};
	int _chunkSize = 1;
	int _compressInputChunkSize{};
	int _filterbankAnalyzeChunkSize{};
	int _compressChannelsChunkSize{};
	int _filterbankSynthesizeChunkSize{};
	int _compressOutputChunkSize{};
	bool _failed{};
public:
	std::string processingLog() const {
		return _processingLog;
	}
	virtual void compressInput(real *, real *, int chunkSize) override
	{
		_compressInputChunkSize = chunkSize;
		_processingLog += "compressInput";
	}
	virtual void analyzeFilterbank(real *, complex *, int chunkSize) override
	{
		_filterbankAnalyzeChunkSize = chunkSize;
		_processingLog += "analyzeFilterbank";
	}
	virtual void compressChannels(complex *, complex *, int chunkSize) override
	{
		_compressChannelsChunkSize = chunkSize;
		_processingLog += "compressChannels";
	}
	virtual void synthesizeFilterbank(complex *, real *, int chunkSize) override
	{
		_filterbankSynthesizeChunkSize = chunkSize;
		_processingLog += "synthesizeFilterbank";
	}
	virtual void compressOutput(real *, real *, int chunkSize) override
	{
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
	void setFailedTrue() {
		_failed = true;
	}
	bool failed() const override {
		return _failed;
	}
};