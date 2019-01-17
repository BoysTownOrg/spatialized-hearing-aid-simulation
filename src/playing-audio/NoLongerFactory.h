#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <memory>
#include <string>
#include <vector>

class RefactoredAudioFrameProcessor {
public:
	INTERFACE_OPERATIONS(RefactoredAudioFrameProcessor);
	virtual void process(gsl::span<gsl::span<float>> audio) = 0;
	virtual int groupDelay() = 0;
	virtual bool complete() = 0;
};
