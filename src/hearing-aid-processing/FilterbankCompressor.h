#pragma once

#include <common-includes/Interface.h>

class FilterbankCompressor {
public:
	INTERFACE_OPERATIONS(FilterbankCompressor);
	virtual void compressInput(
		float *input,
		float *output,
		int chunkSize) = 0;
	virtual void analyzeFilterbank(
		float *input,
		float *output,
		int chunkSize) = 0;
	virtual void compressChannels(
		float *input,
		float *output,
		int chunkSize) = 0;
	virtual void synthesizeFilterbank(
		float *input,
		float *output,
		int chunkSize) = 0;
	virtual void compressOutput(
		float *input,
		float *output,
		int chunkSize) = 0;
};
