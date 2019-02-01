#pragma once

#include <spatialized-hearing-aid-simulation/SignalProcessor.h>

class SignalProcessorStub : public SignalProcessor {
	signal_type signal_{};
	int samples_{};
	int groupDelay_{};
public:
	auto signal() const noexcept {
		return signal_;
	}

	void process(signal_type signal) override {
		signal_ = std::move(signal);
	}

	void setGroupDelay(int n) noexcept {
		groupDelay_ = n;
	}

	index_type groupDelay() override {
		return groupDelay_;
	}
};

class AddsSamplesBy : public SignalProcessor {
	signal_type::element_type c;
public:
	explicit AddsSamplesBy(signal_type::element_type c) noexcept : c{ c } {}

	void process(signal_type signal) override {
		for (auto &x : signal)
			x += c;
	}

	index_type groupDelay() override { return {}; }
};

class MultipliesSamplesBy : public SignalProcessor {
	signal_type::element_type c;
public:
	explicit MultipliesSamplesBy(signal_type::element_type c) noexcept : c{ c } {}

	void process(signal_type signal) override {
		for (auto &x : signal)
			x *= c;
	}

	index_type groupDelay() override { return {}; }
};
