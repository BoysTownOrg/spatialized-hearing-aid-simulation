#pragma once

#include <binaural-room-impulse-response/BrirReader.h>

class BrirReaderStub : public BrirReader {
	std::string filePath_{};
public:
	BinauralRoomImpulseResponse read(std::string filePath) override {
		filePath_ = std::move(filePath);
		return {};
	}

	std::string filePath() const {
		return filePath_;
	}
};

class FailingBrirReader : public BrirReader {
	std::string errorMessage{};
public:
	void setErrorMessage(std::string s) {
		errorMessage = std::move(s);
	}

	BinauralRoomImpulseResponse read(std::string) override {
		throw ReadFailure{ errorMessage };
	}
};