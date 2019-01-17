#include <playing-audio/RefactoredAudioFrameProcessor.h>
#include <common-includes/RuntimeError.h>
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
	RefactoredAudioFrameProcessorFactory::Parameters processing{};
	std::shared_ptr<RefactoredAudioFrameProcessor> processor{};
	std::shared_ptr<AudioFrameReader> reader{};
	AudioFrameReaderFactory *readerFactory;
	RefactoredAudioFrameProcessorFactory *processorFactory;
	int paddedZeroes{};
public:
	RefactoredAudioFrameProcessorImpl(
		AudioFrameReaderFactory *readerFactory, 
		RefactoredAudioFrameProcessorFactory *processorFactory
	) :
		readerFactory{ readerFactory },
		processorFactory{ processorFactory } 
	{
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
		try {
			processorFactory->make(processing);
		}
		catch (const RefactoredAudioFrameProcessorFactory::CreateError &e) {
			throw InitializationFailure{ e.what() };
		}
		processing.channelScalars.clear();
	}

	RUNTIME_ERROR(InitializationFailure);

	struct Preparation {
		std::string audioFilePath;
		double level_dB_Spl;
	};

	void prepare(Preparation p) {
		reader = makeReader(p.audioFilePath);
		const auto desiredRms = std::pow(10.0, (p.level_dB_Spl - processing.max_dB_Spl) / 20.0);
		RmsComputer rms{ *reader };
		for (int i = 0; i < reader->channels(); ++i)
			processing.channelScalars.push_back(desiredRms / rms.compute(i));
		processor = makeProcessor(processing);
		reader->reset();
	}

	RUNTIME_ERROR(PreparationFailure);

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

	int channels() {
		return {};
	}

	int sampleRate() {
		return {};
	}

private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath) {
		try {
			return readerFactory->make(std::move(filePath));
		}
		catch (const AudioFrameReaderFactory::CreateError &e) {
			throw PreparationFailure{ e.what() };
		}
	}

	std::shared_ptr<RefactoredAudioFrameProcessor> makeProcessor(
		RefactoredAudioFrameProcessorFactory::Parameters p
	) {
		try {
			return processorFactory->make(std::move(p));
		}
		catch (const RefactoredAudioFrameProcessorFactory::CreateError &e) {
			throw PreparationFailure{ e.what() };
		}
	}
};

class RefactoredAudioFrameProcessorImplFactory{};

class RefactoredAudioFrameProcessorStub : public RefactoredAudioFrameProcessor {
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

	int groupDelay() override {
		return groupDelay_;
	}

	void setComplete() {
		complete_ = true;
	}

	bool complete() override {
		return complete_;
	}
};

class RefactoredAudioFrameProcessorStubFactory : public RefactoredAudioFrameProcessorFactory {
	Parameters _parameters{};
	std::shared_ptr<RefactoredAudioFrameProcessor> processor;
public:
	explicit RefactoredAudioFrameProcessorStubFactory(
		std::shared_ptr<RefactoredAudioFrameProcessor> processor =
			std::make_shared<RefactoredAudioFrameProcessorStub>()
	) :
		processor{ std::move(processor) } {}

	void setProcessor(std::shared_ptr<RefactoredAudioFrameProcessor> p) {
		this->processor = std::move(p);
	}

	const Parameters &parameters() const {
		return _parameters;
	}

	std::shared_ptr<RefactoredAudioFrameProcessor> make(Parameters p) override {
		_parameters = p;
		return processor;
	}
};

class RefactoredErrorAudioFrameProcessorFactory : public RefactoredAudioFrameProcessorFactory {
	std::string errorMessage{};
public:
	explicit RefactoredErrorAudioFrameProcessorFactory(
		std::string errorMessage
	) :
		errorMessage{ std::move(errorMessage) } {}

	std::shared_ptr<RefactoredAudioFrameProcessor> make(Parameters) override {
		throw CreateError{ errorMessage };
	}
};

#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

namespace {
	class RefactoredAudioFrameProcessorImplTests : public ::testing::Test {
	protected:
		std::shared_ptr<AudioFrameReaderStub> reader =
			std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory readerFactory{reader};
		std::shared_ptr<RefactoredAudioFrameProcessorStub> processor =
			std::make_shared<RefactoredAudioFrameProcessorStub>();
		RefactoredAudioFrameProcessorStubFactory processorFactory{processor};
		RefactoredAudioFrameProcessorImpl impl{ &readerFactory, &processorFactory };
	};

	TEST_F(RefactoredAudioFrameProcessorImplTests, processPassesAudioToReaderAndProcessor) {
		impl.prepare({});
		gsl::span<float> x{};
		impl.process({ &x, 1 });
		EXPECT_EQ(&x, reader->audioBuffer().data());
		EXPECT_EQ(1, reader->audioBuffer().size());
		EXPECT_EQ(&x, processor->audioBuffer().data());
		EXPECT_EQ(1, processor->audioBuffer().size());
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, processPadsZeroToEndOfInput) {
		impl.prepare({});
		reader->setComplete();
		std::vector<float> audio(3, -1);
		gsl::span<float> x{ audio };
		impl.process({ &x, 1 });
		EXPECT_EQ(0, processor->audioBuffer()[0][0]);
		EXPECT_EQ(0, processor->audioBuffer()[0][1]);
		EXPECT_EQ(0, processor->audioBuffer()[0][2]);
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, completeAfterProcessingPaddedZeroes) {
		impl.prepare({});
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

	TEST_F(RefactoredAudioFrameProcessorImplTests, preparePassesAllParametersToFactories) {
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
		impl.prepare(p);
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

	TEST_F(RefactoredAudioFrameProcessorImplTests, preparePassesCalibrationScaleToProcessorFactory) {
		RefactoredAudioFrameProcessorImpl::Initialization initialization;
		initialization.max_dB_Spl = 8;
		impl.initialize(initialization);
		FakeAudioFileReader fake{ { 1, 2, 3, 4, 5, 6 } };
		fake.setChannels(2);
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fake));
		RefactoredAudioFrameProcessorImpl::Preparation p{};
		p.level_dB_Spl = 7;
		impl.prepare(p);
		assertEqual(
			{
				std::pow(10.0, (7 - 8) / 20.0) / std::sqrt((1 * 1 + 3 * 3 + 5 * 5) / 3.0),
				std::pow(10.0, (7 - 8) / 20.0) / std::sqrt((2 * 2 + 4 * 4 + 6 * 6) / 3.0)
			},
			processorFactory.parameters().channelScalars,
			1e-6
		);
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, prepareResetsReaderAfterComputingRms) {
		impl.prepare({});
		EXPECT_TRUE(reader->readingLog().endsWith("reset "));
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, sampleRateAndChannelsReturnedFromReader) {
		reader->setChannels(1);
		reader->setSampleRate(2);
		impl.prepare({});
		EXPECT_EQ(1, impl.channels());
		EXPECT_EQ(2, impl.sampleRate());
	}

	class RefactoredAudioFrameProcessorImplRequestErrorTests : public ::testing::Test {
	protected:
		AudioFrameReaderStubFactory defaultReaderFactory{};
		RefactoredAudioFrameProcessorStubFactory defaultProcessorFactory{};
		AudioFrameReaderFactory *readerFactory{&defaultReaderFactory};
		RefactoredAudioFrameProcessorFactory *processorFactory{&defaultProcessorFactory};

		void assertPrepareThrowsPreparationFailure(std::string what) {
			RefactoredAudioFrameProcessorImpl impl{ readerFactory, processorFactory };
			try {
				impl.prepare({});
				FAIL() << "Expected RefactoredAudioFrameProcessorImpl::PreparationFailure";
			}
			catch (const RefactoredAudioFrameProcessorImpl::PreparationFailure &e) {
				assertEqual(what, e.what());
			}
		}

		void assertInitializeThrowsInitializationFailure(std::string what) {
			RefactoredAudioFrameProcessorImpl impl{ readerFactory, processorFactory };
			try {
				impl.initialize({});
				FAIL() << "Expected RefactoredAudioFrameProcessorImpl::InitializationFailure";
			}
			catch (const RefactoredAudioFrameProcessorImpl::InitializationFailure &e) {
				assertEqual(what, e.what());
			}
		}
	};

	TEST_F(
		RefactoredAudioFrameProcessorImplRequestErrorTests,
		initializeThrowsInitializationFailureWhenProcessorFactoryThrowsCreateError
	) {
		RefactoredErrorAudioFrameProcessorFactory failingFactory{ "error." };
		processorFactory = &failingFactory;
		assertInitializeThrowsInitializationFailure("error.");
	}

	TEST_F(
		RefactoredAudioFrameProcessorImplRequestErrorTests,
		prepareThrowsPreparationFailureWhenProcessorFactoryThrowsCreateError
	) {
		RefactoredErrorAudioFrameProcessorFactory failingFactory{ "error." };
		processorFactory = &failingFactory;
		assertPrepareThrowsPreparationFailure("error.");
	}

	TEST_F(
		RefactoredAudioFrameProcessorImplRequestErrorTests,
		prepareThrowsPreparationFailureWhenReaderFactoryThrowsCreateError
	) {
		ErrorAudioFrameReaderFactory failingFactory{ "error." };
		readerFactory = &failingFactory;
		assertPrepareThrowsPreparationFailure("error.");
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

	class TimesTwo : public RefactoredAudioFrameProcessor {
		void process(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x *= 2;
		}

		int groupDelay() override { return {}; }

		bool complete() override
		{
			return false;
		}
	};

	TEST(RefactoredAudioFrameProcessorOtherTests, processReadsThenProcesses) {
		AudioFrameReaderStubFactory readerFactory{ std::make_shared<ReadsAOne>() };
		RefactoredAudioFrameProcessorStubFactory processorFactory{ std::make_shared<TimesTwo>() };
		RefactoredAudioFrameProcessorImpl impl{ &readerFactory, &processorFactory };
		impl.prepare({});
		float y{};
		gsl::span<float> x{ &y, 1 };
		impl.process({ &x, 1 });
		EXPECT_EQ(2, y);
	}

	TEST(RefactoredAudioFrameProcessorOtherTests, tbd2) {
		RefactoredAudioFrameProcessorImplFactory factory{};
	}
}