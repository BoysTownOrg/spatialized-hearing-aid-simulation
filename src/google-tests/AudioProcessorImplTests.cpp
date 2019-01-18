#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

namespace {
	class AudioProcessorImplTests : public ::testing::Test {
	protected:
		std::shared_ptr<AudioFrameReaderStub> reader =
			std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory readerFactory{reader};
		std::shared_ptr<AudioFrameProcessorStub> processor =
			std::make_shared<AudioFrameProcessorStub>();
		AudioFrameProcessorStubFactory processorFactory{processor};
		AudioProcessorImpl impl{ &readerFactory, &processorFactory };

		void prepare(AudioProcessorImpl::Preparation p = {}) {
			impl.prepare(std::move(p));
		}

		void load(gsl::span<gsl::span<float>> s = {}) {
			impl.load(std::move(s));
		}
	};

	TEST_F(AudioProcessorImplTests, initializePassesParametersToFactoryForExceptionCheck) {
		AudioProcessorImpl::Initialization init;
		init.leftDslPrescriptionFilePath = "a";
		init.rightDslPrescriptionFilePath = "b";
		init.brirFilePath = "c";
		init.max_dB_Spl = 1;
		init.attack_ms = 2;
		init.release_ms = 3;
		init.windowSize = 4;
		init.chunkSize = 5;
		impl.initialize(init);
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

	TEST_F(AudioProcessorImplTests, queriesDoNotThrowIfNotPrepared) {
		reader->setChannels(1);
		reader->setSampleRate(2);
		reader->setIncomplete();
		EXPECT_EQ(0, impl.channels());
		EXPECT_EQ(0, impl.sampleRate());
		EXPECT_EQ(0, impl.chunkSize());
		EXPECT_TRUE(impl.complete());
	}

	TEST_F(AudioProcessorImplTests, chunkSizeReturnsWhatWasInitialized) {
		AudioProcessorImpl::Initialization init;
		init.chunkSize = 5;
		impl.initialize(init);
		EXPECT_EQ(5, impl.chunkSize());
	}

	TEST_F(AudioProcessorImplTests, preparePassesAllParametersToFactories) {
		AudioProcessorImpl::Initialization init;
		init.leftDslPrescriptionFilePath = "a";
		init.rightDslPrescriptionFilePath = "b";
		init.brirFilePath = "c";
		init.max_dB_Spl = 1;
		init.attack_ms = 2;
		init.release_ms = 3;
		init.windowSize = 4;
		init.chunkSize = 5;
		impl.initialize(init);
		AudioProcessorImpl::Preparation p{};
		p.audioFilePath = "d";
		reader->setChannels(6);
		reader->setSampleRate(7);
		prepare(p);
		assertEqual("a", processorFactory.parameters().leftDslPrescriptionFilePath);
		assertEqual("b", processorFactory.parameters().rightDslPrescriptionFilePath);
		assertEqual("c", processorFactory.parameters().brirFilePath);
		assertEqual("d", readerFactory.filePath());
		EXPECT_EQ(1, processorFactory.parameters().max_dB_Spl);
		EXPECT_EQ(2, processorFactory.parameters().attack_ms);
		EXPECT_EQ(3, processorFactory.parameters().release_ms);
		EXPECT_EQ(4, processorFactory.parameters().windowSize);
		EXPECT_EQ(5, processorFactory.parameters().chunkSize);
		EXPECT_EQ(6, processorFactory.parameters().channels);
		EXPECT_EQ(7, processorFactory.parameters().sampleRate);
	}

	TEST_F(AudioProcessorImplTests, preparePassesCalibrationScaleToProcessorFactory) {
		AudioProcessorImpl::Initialization init;
		init.max_dB_Spl = 8;
		impl.initialize(init);
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fakeReader));
		AudioProcessorImpl::Preparation p{};
		p.level_dB_Spl = 7;
		prepare(p);
		assertEqual(
			{
				std::pow(10.0, (7 - 8) / 20.0) / std::sqrt((1 * 1 + 3 * 3 + 5 * 5) / 3.0),
				std::pow(10.0, (7 - 8) / 20.0) / std::sqrt((2 * 2 + 4 * 4 + 6 * 6) / 3.0)
			},
			processorFactory.parameters().channelScalars,
			1e-6
		);
	}

	TEST_F(AudioProcessorImplTests, prepareResetsReaderAfterComputingRms) {
		prepare();
		EXPECT_TRUE(reader->readingLog().endsWith("reset "));
	}

	TEST_F(AudioProcessorImplTests, returnsSampleRateAndChannelsFromReader) {
		reader->setChannels(1);
		reader->setSampleRate(2);
		prepare();
		EXPECT_EQ(1, impl.channels());
		EXPECT_EQ(2, impl.sampleRate());
	}

	TEST_F(AudioProcessorImplTests, processReadsAndProcessesAudio) {
		prepare();
		gsl::span<float> x{};
		load({ &x, 1 });
		EXPECT_EQ(&x, reader->audioBuffer().data());
		EXPECT_EQ(1, reader->audioBuffer().size());
		EXPECT_EQ(&x, processor->audioBuffer().data());
		EXPECT_EQ(1, processor->audioBuffer().size());
	}

	TEST_F(AudioProcessorImplTests, processPadsZeroToEndOfReadInput) {
		prepare();
		reader->setComplete();
		std::vector<float> audio(3, -1);
		gsl::span<float> x{ audio };
		load({ &x, 1 });
		EXPECT_EQ(0, processor->audioBuffer().at(0).at(0));
		EXPECT_EQ(0, processor->audioBuffer().at(0).at(1));
		EXPECT_EQ(0, processor->audioBuffer().at(0).at(2));
	}

	TEST_F(AudioProcessorImplTests, completeAfterProcessingPaddedZeroes) {
		prepare();
		reader->setComplete();
		processor->setGroupDelay(3);
		std::vector<float> y(1);
		std::vector<gsl::span<float>> x{ y, y };
		load(x);
		EXPECT_FALSE(impl.complete());
		load(x);
		EXPECT_FALSE(impl.complete());
		load(x);
		EXPECT_TRUE(impl.complete());
		prepare();
		load(x);
		EXPECT_FALSE(impl.complete());
	}

	TEST_F(AudioProcessorImplTests, preferredProcessingSizesReturnsThatOfProcessorFactory) {
		processorFactory.setPreferredProcessingSizes({ 1, 2, 3 });
		assertEqual({ 1, 2, 3 }, impl.preferredProcessingSizes());
	}

	class ReadsAOne : public AudioFrameReader {
		void read(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x = 1;
		}

		int channels() const override { return 1; }
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

	TEST_F(AudioProcessorImplTests, processReadsThenProcesses) {
		readerFactory.setReader(std::make_shared<ReadsAOne>());
		processorFactory.setProcessor(std::make_shared<TimesTwo>());
		prepare();
		float y{};
		gsl::span<float> x{ &y, 1 };
		impl.load({ &x, 1 });
		EXPECT_EQ(2, y);
	}

	class AudioProcessorImplErrorTests : public ::testing::Test {
	protected:
		AudioFrameReaderStubFactory defaultReaderFactory{};
		AudioFrameProcessorStubFactory defaultProcessorFactory{};
		AudioFrameReaderFactory *readerFactory{&defaultReaderFactory};
		AudioFrameProcessorFactory *processorFactory{&defaultProcessorFactory};

		void assertPrepareThrowsPreparationFailure(std::string what) {
			AudioProcessorImpl impl{ readerFactory, processorFactory };
			try {
				impl.prepare({});
				FAIL() << "Expected AudioProcessorImpl::PreparationFailure";
			}
			catch (const AudioProcessorImpl::PreparationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertInitializeThrowsInitializationFailure(std::string what) {
			AudioProcessorImpl impl{ readerFactory, processorFactory };
			try {
				impl.initialize({});
				FAIL() << "Expected AudioProcessorImpl::InitializationFailure";
			}
			catch (const AudioProcessorImpl::InitializationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
	};

	TEST_F(
		AudioProcessorImplErrorTests,
		initializeThrowsInitializationFailureWhenProcessorFactoryThrowsCreateError
	) {
		CreatingErrorAudioFrameProcessorFactory failingFactory{ "error." };
		processorFactory = &failingFactory;
		assertInitializeThrowsInitializationFailure("error.");
	}

	TEST_F(
		AudioProcessorImplErrorTests,
		prepareThrowsPreparationFailureWhenProcessorFactoryThrowsCreateError
	) {
		CreatingErrorAudioFrameProcessorFactory failingFactory{ "error." };
		processorFactory = &failingFactory;
		assertPrepareThrowsPreparationFailure("error.");
	}

	TEST_F(
		AudioProcessorImplErrorTests,
		prepareThrowsPreparationFailureWhenReaderFactoryThrowsCreateError
	) {
		ErrorAudioFrameReaderFactory failingFactory{ "error." };
		readerFactory = &failingFactory;
		assertPrepareThrowsPreparationFailure("error.");
	}
}