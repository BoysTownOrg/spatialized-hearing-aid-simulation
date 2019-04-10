#pragma once

#include <spatialized-hearing-aid-simulation/SignalProcessor.h>
#include <vector>

class SignalProcessorStub : public SignalProcessor {
	std::vector<signal_type::element_type> processed_{};
	int samples_{};
	int groupDelay_{};
public:
	void process(signal_type signal) override {
		std::copy(signal.begin(), signal.end(), std::back_inserter(processed_));
	}

	auto processed() const {
		return processed_;
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
