#pragma once

#include "FilterbankCompressor.h"
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <memory>
#include <vector>

#ifdef HEARING_AID_PROCESSING_EXPORTS
	#define HEARING_AID_PROCESSING_API __declspec(dllexport)
#else
	#define HEARING_AID_PROCESSING_API __declspec(dllimport)
#endif

class HearingAidProcessor {
	// Order important for construction.
	std::vector<FilterbankCompressor::complex_type> buffer;
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	using signal_type = gsl::span<float>;
	using index_type = signal_type::index_type;
	HEARING_AID_PROCESSING_API explicit HearingAidProcessor(
		std::shared_ptr<FilterbankCompressor>
	);
	RUNTIME_ERROR(CompressorError);
	HEARING_AID_PROCESSING_API void process(signal_type);
	HEARING_AID_PROCESSING_API index_type groupDelay();
private:
	void throwIfNotPowerOfTwo(int n, std::string name);
};

