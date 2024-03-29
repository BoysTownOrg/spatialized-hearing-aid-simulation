#pragma once

#include <spatialized-hearing-aid-simulation/StimulusList.h>
#include <vector>

class FakeStimulusList : public StimulusList {
	std::vector<std::string> contents_{};
	std::string directory_{};
public:
	void setContents(std::vector<std::string> v) noexcept {
		contents_ = std::move(v);
	}

	auto directory() const {
		return directory_;
	}

	void initialize(std::string d) override {
		directory_ = std::move(d);
	}

	std::string next() override {
		if (contents_.size()) {
			auto next_ = contents_.front();
			contents_.erase(contents_.begin());
			return next_;
		}
		else
			return {};
	}

	bool empty() override {
		return contents_.empty();
	}
};

class FailsToInitializeStimulusList : public StimulusList {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	void initialize(std::string) override {
		throw InitializationFailure{ errorMessage };
	}

	std::string next() override { return {}; }

	bool empty() override { return {}; }
};