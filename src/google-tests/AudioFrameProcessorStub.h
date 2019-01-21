#pragma once
#include <playing-audio/AudioProcessingLoader.h>

class AudioFrameProcessorStub : public AudioFrameProcessor {
	gsl::span<gsl::span<float>> _audioBuffer{};
	int groupDelay_{};
	bool complete_{};
public:
	const gsl::span<gsl::span<float>> audioBuffer() const {
		return _audioBuffer;
	}

	void process(gsl::span<gsl::span<float>> audio) override {
		_audioBuffer = audio;
	}

	void setGroupDelay(int n) {
		groupDelay_ = n;
	}

	channel_type::index_type groupDelay() override {
		return groupDelay_;
	}

	void setComplete() {
		complete_ = true;
	}
};

class AudioFrameProcessorStubFactory : public AudioFrameProcessorFactory {
	std::vector<int> preferredProcessingSizes_{};
	Parameters parameters_{};
	std::shared_ptr<AudioFrameProcessor> processor;
public:
	explicit AudioFrameProcessorStubFactory(
		std::shared_ptr<AudioFrameProcessor> processor =
			std::make_shared<AudioFrameProcessorStub>()
	) :
		processor{ std::move(processor) } {}

	void setProcessor(std::shared_ptr<AudioFrameProcessor> p) {
		this->processor = std::move(p);
	}

	const Parameters &parameters() const {
		return parameters_;
	}

	std::shared_ptr<AudioFrameProcessor> make(Parameters p) override {
		parameters_ = std::move(p);
		return processor;
	}

	void setPreferredProcessingSizes(std::vector<int> v) {
		preferredProcessingSizes_ = std::move(v);
	}

	std::vector<int> preferredProcessingSizes() override {
		return preferredProcessingSizes_;
	}
};

class CreatingErrorAudioFrameProcessorFactory : public AudioFrameProcessorFactory {
	std::string errorMessage{};
public:
	explicit CreatingErrorAudioFrameProcessorFactory(
		std::string errorMessage
	) :
		errorMessage{ std::move(errorMessage) } {}

	std::shared_ptr<AudioFrameProcessor> make(Parameters) override {
		throw CreateError{ errorMessage };
	}

	std::vector<int> preferredProcessingSizes() override {
		return {};
	}
};