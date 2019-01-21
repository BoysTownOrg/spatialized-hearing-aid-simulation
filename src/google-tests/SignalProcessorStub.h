#pragma once

#include <signal-processing/SignalProcessor.h>

class SignalProcessorStub : public SignalProcessor {
	signal_type _signal{};
	int _samples{};
	int groupDelay_{};
public:
	const signal_type signal() const {
		return _signal;
	}

	void process(signal_type signal) override {
		_signal = std::move(signal);
	}

	void setGroupDelay(int n) {
		groupDelay_ = n;
	}

	index_type groupDelay() override {
		return groupDelay_;
	}
};
