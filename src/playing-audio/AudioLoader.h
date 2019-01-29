#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <string>
#include <vector>

class AudioLoader {
public:
	virtual void reset() = 0;
	virtual bool complete() = 0;
	using channel_type = gsl::span<float>;
	virtual void load(gsl::span<channel_type> audio) = 0;
};