#pragma once

#include <spatialized-hearing-aid-simulation/BrirReader.h>

class BrirReaderStub : public BrirReader {
	BinauralRoomImpulseResponse brir_{};
	std::string filePath_{};
	bool readCalled_{};
public:
	void setBrir(BinauralRoomImpulseResponse brir) noexcept {
		brir_ = std::move(brir);
	}

	BinauralRoomImpulseResponse read(std::string filePath) override {
		readCalled_ = true;
		filePath_ = std::move(filePath);
		return brir_;
	}

	auto filePath() const {
		return filePath_;
	}

	auto readCalled() const noexcept {
		return readCalled_;
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