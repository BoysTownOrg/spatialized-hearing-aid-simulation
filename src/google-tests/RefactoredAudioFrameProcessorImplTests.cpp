#include <playing-audio/AudioFrameProcessor.h>
#include <audio-stream-processing/AudioFrameReader.h>

class RmsComputer {
	std::vector<std::vector<float>> entireAudioFile;
public:
	explicit RmsComputer(AudioFrameReader &reader) :
		entireAudioFile{ reader.channels() }
	{
		std::vector<gsl::span<float>> pointers;
		for (auto &channel : entireAudioFile) {
			channel.resize(gsl::narrow<std::vector<float>::size_type>(reader.frames()));
			pointers.push_back({ channel });
		}
		reader.read(pointers);
	}

	double compute(int channel) {
		double squaredSum{};
		const auto channel_ = entireAudioFile.at(channel);
		for (const auto sample : channel_)
			squaredSum += sample * sample;
		return std::sqrt(squaredSum / channel_.size());
	}
};

class RefactoredAudioFrameProcessorImpl {
	AudioFrameProcessorFactory::Parameters processing{};
	std::shared_ptr<AudioFrameProcessor> processor{};
	std::shared_ptr<AudioFrameReader> reader{};
	AudioFrameReaderFactory *readerFactory;
	AudioFrameProcessorFactory *processorFactory;
	int paddedZeroes{};
public:
	RefactoredAudioFrameProcessorImpl(
		AudioFrameReaderFactory *readerFactory, 
		AudioFrameProcessorFactory *processorFactory
	) :
		readerFactory{ readerFactory },
		processorFactory{ processorFactory } 
	{
		reader = readerFactory->make({});
		processor = processorFactory->make({});
	}

	struct Initialization {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		double max_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};

	void initialize(Initialization initialization) {
		processing.attack_ms = initialization.attack_ms;
		processing.release_ms = initialization.release_ms;
		processing.brirFilePath = initialization.brirFilePath;
		processing.leftDslPrescriptionFilePath = initialization.leftDslPrescriptionFilePath;
		processing.rightDslPrescriptionFilePath = initialization.rightDslPrescriptionFilePath;
		processing.chunkSize = initialization.chunkSize;
		processing.windowSize = initialization.windowSize;
		processing.max_dB_Spl = initialization.max_dB_Spl;
		processing.channelScalars.resize(2);
		processorFactory->make(processing);
		processing.channelScalars.clear();
	}

	struct Preparation {
		std::string audioFilePath;
		double level_dB_Spl;
	};

	void read(Preparation p) {
		reader = readerFactory->make(p.audioFilePath);
		const auto desiredRms = std::pow(10.0, (p.level_dB_Spl - processing.max_dB_Spl) / 20.0);
		RmsComputer rms{ *reader };
		for (int i = 0; i < reader->channels(); ++i)
			processing.channelScalars.push_back(desiredRms / rms.compute(i));
		processorFactory->make(processing);
	}

	void process(gsl::span<gsl::span<float>> audio) {
		if (reader->complete()) {
			for (auto channel : audio)
				for (auto &x : channel) {
					++paddedZeroes;
					x = 0;
				}
		}
		reader->read(audio);
		processor->process(audio);
	}

	bool complete() {
		return paddedZeroes >= processor->groupDelay();
	}
};

class RefactoredAudioFrameProcessorImplFactory{};

#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

namespace {
	class RefactoredAudioFrameProcessorImplTests : public ::testing::Test {
	protected:
		std::shared_ptr<AudioFrameReaderStub> reader =
			std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory readerFactory{reader};
		std::shared_ptr<AudioFrameProcessorStub> processor =
			std::make_shared<AudioFrameProcessorStub>();
		AudioFrameProcessorStubFactory processorFactory{processor};
		RefactoredAudioFrameProcessorImpl impl{ &readerFactory, &processorFactory };
	};

	TEST_F(RefactoredAudioFrameProcessorImplTests, processPassesAudioToReaderAndProcessor) {
		gsl::span<float> x{};
		impl.process({ &x, 1 });
		EXPECT_EQ(&x, reader->audioBuffer().data());
		EXPECT_EQ(1, reader->audioBuffer().size());
		EXPECT_EQ(&x, processor->audioBuffer().data());
		EXPECT_EQ(1, processor->audioBuffer().size());
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, processPadsZeroToEndOfInput) {
		reader->setComplete();
		std::vector<float> audio(3, -1);
		gsl::span<float> x{ audio };
		impl.process({ &x, 1 });
		EXPECT_EQ(0, processor->audioBuffer()[0][0]);
		EXPECT_EQ(0, processor->audioBuffer()[0][1]);
		EXPECT_EQ(0, processor->audioBuffer()[0][2]);
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, completeAfterProcessingPaddedZeroes) {
		reader->setComplete();
		processor->setGroupDelay(3);
		float y{};
		gsl::span<float> x{ &y, 1 };
		impl.process({ &x, 1 });
		EXPECT_FALSE(impl.complete());
		impl.process({ &x, 1 });
		EXPECT_FALSE(impl.complete());
		impl.process({ &x, 1 });
		EXPECT_TRUE(impl.complete());
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, initializePassesParametersToFactory) {
		RefactoredAudioFrameProcessorImpl::Initialization initialization;
		initialization.leftDslPrescriptionFilePath = "a";
		initialization.rightDslPrescriptionFilePath = "b";
		initialization.brirFilePath = "c";
		initialization.max_dB_Spl = 1;
		initialization.attack_ms = 2;
		initialization.release_ms = 3;
		initialization.windowSize = 4;
		initialization.chunkSize = 5;
		impl.initialize(initialization);
		assertEqual("a", processorFactory.parameters().leftDslPrescriptionFilePath);
		assertEqual("b", processorFactory.parameters().rightDslPrescriptionFilePath);
		assertEqual("c", processorFactory.parameters().brirFilePath);
		EXPECT_EQ(1, processorFactory.parameters().max_dB_Spl);
		EXPECT_EQ(2, processorFactory.parameters().attack_ms);
		EXPECT_EQ(3, processorFactory.parameters().release_ms);
		EXPECT_EQ(4, processorFactory.parameters().windowSize);
		EXPECT_EQ(5, processorFactory.parameters().chunkSize);
		EXPECT_EQ(2U, processorFactory.parameters().channelScalars.size());
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, readPassesParametersToFactories) {
		RefactoredAudioFrameProcessorImpl::Initialization initialization;
		initialization.leftDslPrescriptionFilePath = "a";
		initialization.rightDslPrescriptionFilePath = "b";
		initialization.brirFilePath = "c";
		initialization.max_dB_Spl = 1;
		initialization.attack_ms = 2;
		initialization.release_ms = 3;
		initialization.windowSize = 4;
		initialization.chunkSize = 5;
		impl.initialize(initialization);
		RefactoredAudioFrameProcessorImpl::Preparation p{};
		p.audioFilePath = "d";
		impl.read(p);
		assertEqual("a", processorFactory.parameters().leftDslPrescriptionFilePath);
		assertEqual("b", processorFactory.parameters().rightDslPrescriptionFilePath);
		assertEqual("c", processorFactory.parameters().brirFilePath);
		assertEqual("d", readerFactory.filePath());
		EXPECT_EQ(1, processorFactory.parameters().max_dB_Spl);
		EXPECT_EQ(2, processorFactory.parameters().attack_ms);
		EXPECT_EQ(3, processorFactory.parameters().release_ms);
		EXPECT_EQ(4, processorFactory.parameters().windowSize);
		EXPECT_EQ(5, processorFactory.parameters().chunkSize);
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, playPassesCalibrationScaleToProcessorFactory) {
		RefactoredAudioFrameProcessorImpl::Initialization initialization;
		initialization.max_dB_Spl = 8;
		impl.initialize(initialization);
		FakeAudioFileReader fake{ { 1, 2, 3, 4, 5, 6 } };
		fake.setChannels(2);
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fake));
		RefactoredAudioFrameProcessorImpl::Preparation p{};
		p.level_dB_Spl = 7;
		impl.read(p);
		assertEqual(
			{
				std::pow(10.0, (7 - 8) / 20.0) / std::sqrt((1 * 1 + 3 * 3 + 5 * 5) / 3.0),
				std::pow(10.0, (7 - 8) / 20.0) / std::sqrt((2 * 2 + 4 * 4 + 6 * 6) / 3.0)
			},
			processorFactory.parameters().channelScalars,
			1e-6
		);
	}

	class ReadsAOne : public AudioFrameReader {
		void read(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x = 1;
		}

		int channels() const override {
			return 1;
		}

		bool complete() const override { return {}; }
		int sampleRate() const override { return {}; }
		long long frames() const override { return {}; }
		void reset() override {}
	};

	class TimesTwo : public AudioFrameProcessor {
		void process(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x *= 2;
		}

		int groupDelay() override { return {}; }
	};

	TEST(RefactoredAudioFrameProcessorOtherTests, processReadsThenProcesses) {
		AudioFrameReaderStubFactory readerFactory{ std::make_shared<ReadsAOne>() };
		AudioFrameProcessorStubFactory processorFactory{ std::make_shared<TimesTwo>() };
		RefactoredAudioFrameProcessorImpl impl{ &readerFactory, &processorFactory };
		float y{};
		gsl::span<float> x{ &y, 1 };
		impl.process({ &x, 1 });
		EXPECT_EQ(2, y);
	}

	TEST(RefactoredAudioFrameProcessorOtherTests, tbd2) {
		RefactoredAudioFrameProcessorImplFactory factory{};
	}
}