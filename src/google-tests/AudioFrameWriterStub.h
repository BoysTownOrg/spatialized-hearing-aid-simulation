#pragma once

#include <spatialized-hearing-aid-simulation/SpatialHearingAidModel.h>

class AudioFrameWriterStub : public AudioFrameWriter {
	std::vector<float> written_{};
public:
	auto written() const {
		return written_;
	}

	void write(gsl::span<channel_type> audio) override {
		for (auto channel : audio)
			for (auto x : channel)
				written_.push_back(x);
	}
};

class AudioFrameWriterStubFactory : public AudioFrameWriterFactory {
	std::string filePath_{};
	std::shared_ptr<AudioFrameWriter> writer;
public:
	explicit AudioFrameWriterStubFactory(
		std::shared_ptr<AudioFrameWriter> writer =
			std::make_shared<AudioFrameWriterStub>()
	) noexcept :
		writer{ std::move(writer) } {}

	std::shared_ptr<AudioFrameWriter> make(std::string filePath) {
		filePath_ = std::move(filePath);
		return writer;
	}

	auto filePath() const {
		return filePath_;
	}
};

class ErrorAudioFrameWriterFactory : public AudioFrameWriterFactory {
	std::string errorMessage{};
public:
	explicit ErrorAudioFrameWriterFactory(
		std::string errorMessage
	) noexcept : 
		errorMessage{ std::move(errorMessage) } {}

	std::shared_ptr<AudioFrameWriter> make(std::string) {
		throw CreateError{ errorMessage };
	}
};