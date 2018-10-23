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

#ifdef HEARING_AID_PROCESSING_EXPORTS
	#define HEARING_AID_PROCESSING_API __declspec(dllexport)
#else
	#define HEARING_AID_PROCESSING_API __declspec(dllimport)
#endif

#include <memory>

class HearingAidProcessor {
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	HEARING_AID_PROCESSING_API explicit HearingAidProcessor(
		std::shared_ptr<FilterbankCompressor> compressor
	);
	HEARING_AID_PROCESSING_API void process();
};

