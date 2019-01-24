#pragma once
#include <playing-audio/AudioProcessingLoader.h>

class AudioFrameProcessorStub : public AudioFrameProcessor {
	gsl::span<channel_type> audioBuffer_{};
	int groupDelay_{};
	bool complete_{};
public:
	const gsl::span<channel_type> audioBuffer() const {
		return audioBuffer_;
	}

	void process(gsl::span<channel_type> audio) override {
		audioBuffer_ = audio;
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
	double fullScale_dB_Spl_{};
	int preferredBufferSize_{};
public:
	explicit AudioFrameProcessorStubFactory(
		std::shared_ptr<AudioFrameProcessor> processor =
			std::make_shared<AudioFrameProcessorStub>()
	) :
		processor{ std::move(processor) } {}

	void setProcessor(std::shared_ptr<AudioFrameProcessor> p) {
		processor = std::move(p);
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

	void setPreferredBufferSize(int n) {
		preferredBufferSize_ = n;
	}

	int preferredBufferSize() override {
		return preferredBufferSize_;
	}

	void setFullScale_dB_Spl(double x) {
		fullScale_dB_Spl_ = x;
	}

	double fullScale_dB_Spl() override { 
		return fullScale_dB_Spl_; 
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

	std::vector<int> preferredProcessingSizes() override { return {}; }
	int preferredBufferSize() override { return {}; }
	double fullScale_dB_Spl() override { return {}; }
};