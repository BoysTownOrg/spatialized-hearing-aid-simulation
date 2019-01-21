#pragma once

#include <recognition-test/StimulusList.h>

class StimulusListStub : public StimulusList {
	std::string directory_{};
	std::string next_{};
	bool empty_{};
	bool nextCalled_{};
public:
	void setNext(std::string s) {
		next_ = std::move(s);
	}

	std::string directory() const {
		return directory_;
	}

	void initialize(std::string d) override {
		directory_ = std::move(d);
	}

	std::string next() override {
		nextCalled_ = true;
		return next_;
	}

	void setEmpty() {
		empty_ = true;
	}

	bool empty() override {
		return empty_;
	}

	bool nextCalled() const {
		return nextCalled_;
	}
};