#pragma once

#include <spatialized-hearing-aid-simulation/SpatialHearingAidModel.h>

class AudioFrameWriterStub : public AudioFrameWriter {
	std::vector<AudioFrameWriter::channel_type::element_type> written_{};
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
    AudioFrameWriter::AudioFormat format_{};
    std::string filePath_{};
	std::shared_ptr<AudioFrameWriter> writer;
public:
	explicit AudioFrameWriterStubFactory(
		std::shared_ptr<AudioFrameWriter> writer =
			std::make_shared<AudioFrameWriterStub>()
	) noexcept :
		writer{ std::move(writer) } {}

	std::shared_ptr<AudioFrameWriter> make(
        std::string filePath,
        const AudioFrameWriter::AudioFormat &format
    ) override {
		filePath_ = std::move(filePath);
        format_ = format;
		return writer;
	}

	auto filePath() const {
		return filePath_;
	}
 
    auto &format() const {
        return format_;
    }
};

class ErrorAudioFrameWriterFactory : public AudioFrameWriterFactory {
	std::string errorMessage{};
public:
	explicit ErrorAudioFrameWriterFactory(
		std::string errorMessage = {}
	) noexcept : 
		errorMessage{ std::move(errorMessage) } {}

	std::shared_ptr<AudioFrameWriter> make(
        std::string,
        const AudioFrameWriter::AudioFormat &
    ) override {
		throw CreateError{ errorMessage };
	}
};
