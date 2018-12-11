#pragma once

#include <signal-processing/SignalProcessor.h>

class SignalProcessorStub : public SignalProcessor {
	gsl::span<float> _signal{};
	int _samples{};
public:
	const gsl::span<float> signal() const {
		return _signal;
	}

	void process(gsl::span<float> signal) override {
		_signal = signal;
	}
};
