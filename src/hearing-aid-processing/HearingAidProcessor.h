#pragma once

#include "FilterbankCompressor.h"
#include <common-includes/RuntimeError.h>
#include <signal-processing/SignalProcessor.h>
#include <memory>
#include <vector>

#ifdef HEARING_AID_PROCESSING_EXPORTS
	#define HEARING_AID_PROCESSING_API __declspec(dllexport)
#else
	#define HEARING_AID_PROCESSING_API __declspec(dllimport)
#endif

class HearingAidProcessor : public SignalProcessor {
	// Order important for construction.
	std::vector<float> buffer;
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	HEARING_AID_PROCESSING_API explicit HearingAidProcessor(
		std::shared_ptr<FilterbankCompressor>
	);
	RUNTIME_ERROR(CompressorError);
	HEARING_AID_PROCESSING_API void process(signal_type) override;
	index_type groupDelay() override;
private:
	void throwIfNotPowerOfTwo(int n, std::string name);
};

