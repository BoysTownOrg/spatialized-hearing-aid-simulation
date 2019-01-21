#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

namespace {
	class AudioProcessingLoaderTests : public ::testing::Test {
	protected:
		std::shared_ptr<AudioFrameReaderStub> reader =
			std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory readerFactory{reader};
		std::shared_ptr<AudioFrameProcessorStub> processor =
			std::make_shared<AudioFrameProcessorStub>();
		AudioFrameProcessorStubFactory processorFactory{processor};
		AudioProcessingLoader loader{ &readerFactory, &processorFactory };

		void prepare(AudioProcessingLoader::Preparation p = {}) {
			loader.prepare(std::move(p));
		}

		void load(gsl::span<gsl::span<float>> s = {}) {
			loader.load(std::move(s));
		}
	};

	TEST_F(AudioProcessingLoaderTests, initializePassesParametersToFactoryForExceptionCheck) {
		AudioProcessingLoader::Initialization init;
		init.leftDslPrescriptionFilePath = "a";
		init.rightDslPrescriptionFilePath = "b";
		init.brirFilePath = "c";
		init.max_dB_Spl = 1;
		init.attack_ms = 2;
		init.release_ms = 3;
		init.windowSize = 4;
		init.chunkSize = 5;
		loader.initialize(init);
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

	TEST_F(AudioProcessingLoaderTests, queriesDoNotThrowIfNotPrepared) {
		reader->setChannels(1);
		reader->setSampleRate(2);
		reader->setIncomplete();
		EXPECT_EQ(0, loader.channels());
		EXPECT_EQ(0, loader.sampleRate());
		EXPECT_EQ(0, loader.chunkSize());
		EXPECT_TRUE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, chunkSizeReturnsWhatWasInitialized) {
		AudioProcessingLoader::Initialization init;
		init.chunkSize = 5;
		loader.initialize(init);
		EXPECT_EQ(5, loader.chunkSize());
	}

	TEST_F(AudioProcessingLoaderTests, preparePassesAllParametersToFactories) {
		AudioProcessingLoader::Initialization init;
		init.leftDslPrescriptionFilePath = "a";
		init.rightDslPrescriptionFilePath = "b";
		init.brirFilePath = "c";
		init.max_dB_Spl = 1;
		init.attack_ms = 2;
		init.release_ms = 3;
		init.windowSize = 4;
		init.chunkSize = 5;
		loader.initialize(init);
		AudioProcessingLoader::Preparation p{};
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

	TEST_F(AudioProcessingLoaderTests, preparePassesCalibrationScaleToProcessorFactory) {
		AudioProcessingLoader::Initialization init;
		init.max_dB_Spl = 8;
		loader.initialize(init);
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fakeReader));
		AudioProcessingLoader::Preparation p{};
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

	TEST_F(AudioProcessingLoaderTests, prepareResetsReaderAfterComputingRms) {
		prepare();
		EXPECT_TRUE(reader->readingLog().endsWith("reset "));
	}

	TEST_F(AudioProcessingLoaderTests, returnsSampleRateAndChannelsFromReader) {
		reader->setChannels(1);
		reader->setSampleRate(2);
		prepare();
		EXPECT_EQ(1, loader.channels());
		EXPECT_EQ(2, loader.sampleRate());
	}

	TEST_F(AudioProcessingLoaderTests, processReadsAndProcessesAudio) {
		prepare();
		gsl::span<float> x{};
		load({ &x, 1 });
		EXPECT_EQ(&x, reader->audioBuffer().data());
		EXPECT_EQ(1, reader->audioBuffer().size());
		EXPECT_EQ(&x, processor->audioBuffer().data());
		EXPECT_EQ(1, processor->audioBuffer().size());
	}

	TEST_F(AudioProcessingLoaderTests, processPadsZeroToEndOfReadInput) {
		prepare();
		reader->setRemainingFrames(2);
        std::vector<float> mono{ -1, -1, -1 };
		gsl::span<float> audio{ mono };
		load({ &audio, 1 });
		EXPECT_EQ(-1, processor->audioBuffer().at(0).at(0));
		EXPECT_EQ(-1, processor->audioBuffer().at(0).at(1));
		EXPECT_EQ(0, processor->audioBuffer().at(0).at(2));
	}

	TEST_F(AudioProcessingLoaderTests, completeAfterProcessingPaddedZeroes) {
		prepare();
		reader->setRemainingFrames(2);
		processor->setGroupDelay(11);
		std::vector<float> y(6);
		std::vector<gsl::span<float>> x{ y, y };
		load(x);
		EXPECT_FALSE(loader.complete());
		reader->setRemainingFrames(0);
		load(x);
		EXPECT_FALSE(loader.complete());
		load(x);
		EXPECT_TRUE(loader.complete());
		prepare();
		reader->setRemainingFrames(2);
		load(x);
		EXPECT_FALSE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, preferredProcessingSizesReturnsThatOfProcessorFactory) {
		processorFactory.setPreferredProcessingSizes({ 1, 2, 3 });
		assertEqual({ 1, 2, 3 }, loader.preferredProcessingSizes());
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
        long long framesRemaining() override { return 1; }
	};

	class TimesTwo : public AudioFrameProcessor {
		void process(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x *= 2;
		}

		int groupDelay() override { return {}; }
	};

	TEST_F(AudioProcessingLoaderTests, processReadsThenProcesses) {
		readerFactory.setReader(std::make_shared<ReadsAOne>());
		processorFactory.setProcessor(std::make_shared<TimesTwo>());
		prepare();
		float y{};
		gsl::span<float> x{ &y, 1 };
		loader.load({ &x, 1 });
		EXPECT_EQ(2, y);
	}

	class AudioProcessingLoaderErrorTests : public ::testing::Test {
	protected:
		AudioFrameReaderStubFactory defaultReaderFactory{};
		AudioFrameProcessorStubFactory defaultProcessorFactory{};
		AudioFrameReaderFactory *readerFactory{&defaultReaderFactory};
		AudioFrameProcessorFactory *processorFactory{&defaultProcessorFactory};

		void assertPrepareThrowsPreparationFailure(std::string what) {
			AudioProcessingLoader loader{ readerFactory, processorFactory };
			try {
				loader.prepare({});
				FAIL() << "Expected AudioProcessingLoader::PreparationFailure";
			}
			catch (const AudioProcessingLoader::PreparationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}

		void assertInitializeThrowsInitializationFailure(std::string what) {
			AudioProcessingLoader loader{ readerFactory, processorFactory };
			try {
				loader.initialize({});
				FAIL() << "Expected AudioProcessingLoader::InitializationFailure";
			}
			catch (const AudioProcessingLoader::InitializationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
	};

	TEST_F(
		AudioProcessingLoaderErrorTests,
		initializeThrowsInitializationFailureWhenProcessorFactoryThrowsCreateError
	) {
		CreatingErrorAudioFrameProcessorFactory failingFactory{ "error." };
		processorFactory = &failingFactory;
		assertInitializeThrowsInitializationFailure("error.");
	}

	TEST_F(
		AudioProcessingLoaderErrorTests,
		prepareThrowsPreparationFailureWhenProcessorFactoryThrowsCreateError
	) {
		CreatingErrorAudioFrameProcessorFactory failingFactory{ "error." };
		processorFactory = &failingFactory;
		assertPrepareThrowsPreparationFailure("error.");
	}

	TEST_F(
		AudioProcessingLoaderErrorTests,
		prepareThrowsPreparationFailureWhenReaderFactoryThrowsCreateError
	) {
		ErrorAudioFrameReaderFactory failingFactory{ "error." };
		readerFactory = &failingFactory;
		assertPrepareThrowsPreparationFailure("error.");
	}
}