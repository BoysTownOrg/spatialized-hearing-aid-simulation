#pragma once

#include <signal-processing/SignalProcessor.h>

class MockSignalProcessor : public SignalProcessor {
	float *_signal{};
	int _samples{};
public:
	const float *signal() const {
		return _signal;
	}

	int samples() const {
		return _samples;
	}

	void process(float *x, int samples) override {
		_signal = x;
		_samples = samples;
	}
};
