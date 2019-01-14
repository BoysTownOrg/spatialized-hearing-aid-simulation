#pragma once

#ifdef HEARING_AID_PROCESSING_EXPORTS
	#define HEARING_AID_PROCESSING_API __declspec(dllexport)
#else
	#define HEARING_AID_PROCESSING_API __declspec(dllimport)
#endif

#include "FilterbankCompressor.h"
#include <common-includes/RuntimeError.h>
#include <signal-processing/SignalProcessor.h>
#include <memory>
#include <vector>

class HearingAidProcessor : public SignalProcessor {
	// Order important for construction.
	std::vector<float> buffer;
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	RUNTIME_ERROR(CompressorError);
	HEARING_AID_PROCESSING_API explicit HearingAidProcessor(
		std::shared_ptr<FilterbankCompressor>
	);
	HEARING_AID_PROCESSING_API void process(gsl::span<float>) override;
};

