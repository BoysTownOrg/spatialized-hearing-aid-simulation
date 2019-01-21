#pragma once

#include <signal-processing/SignalProcessor.h>

class SignalProcessorStub : public SignalProcessor {
	signal_type signal_{};
	int samples_{};
	int groupDelay_{};
public:
	const signal_type signal() const {
		return signal_;
	}

	void process(signal_type signal) override {
		signal_ = std::move(signal);
	}

	void setGroupDelay(int n) {
		groupDelay_ = n;
	}

	index_type groupDelay() override {
		return groupDelay_;
	}
};
