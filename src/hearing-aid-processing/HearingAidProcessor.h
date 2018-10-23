#pragma once

#ifdef HEARING_AID_PROCESSING_EXPORTS
	#define HEARING_AID_PROCESSING_API __declspec(dllexport)
#else
	#define HEARING_AID_PROCESSING_API __declspec(dllimport)
#endif

#include "FilterbankCompressor.h"
#include <memory>

class HearingAidProcessor {
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	HEARING_AID_PROCESSING_API explicit HearingAidProcessor(
		std::shared_ptr<FilterbankCompressor> compressor
	);
	HEARING_AID_PROCESSING_API void process();
};

