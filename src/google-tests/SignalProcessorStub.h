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

class AddOne : public SignalProcessor {
public:
	void process(signal_type signal) override {
		for (auto &x : signal)
			x += 1;
	}

	index_type groupDelay() override { return {}; }
};

class TimesTwo : public SignalProcessor {
public:
	void process(signal_type signal) override {
		for (auto &x : signal)
			x *= 2;
	}

	index_type groupDelay() override { return {}; }
};
