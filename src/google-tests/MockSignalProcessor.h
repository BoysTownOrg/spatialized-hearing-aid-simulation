#pragma once

#include <signal-processing/SignalProcessor.h>

class MockSignalProcessor : public SignalProcessor {
	float *_signal{};
	int _frameCount{};
public:
	const float *signal() const {
		return _signal;
	}
	int frames() const {
		return _frameCount;
	}
	void process(float *x, int count) override {
		_signal = x;
		_frameCount = count;
	}
};
