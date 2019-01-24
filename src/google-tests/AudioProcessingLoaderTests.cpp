#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
	class AudioProcessingLoaderTests : public ::testing::Test {
	protected:
		AudioProcessingLoader::Initialization initialization{};
		AudioProcessingLoader::Preparation preparation{};
		using vector_type = std::vector<float>;
		vector_type left{};
		vector_type right{};
		std::shared_ptr<AudioFrameReaderStub> reader =
			std::make_shared<AudioFrameReaderStub>();
		AudioFrameReaderStubFactory readerFactory{reader};
		std::shared_ptr<AudioFrameProcessorStub> processor =
			std::make_shared<AudioFrameProcessorStub>();
		AudioFrameProcessorStubFactory processorFactory{processor};
		AudioProcessingLoader loader{ &readerFactory, &processorFactory };

		void initialize() {
			loader.initialize(initialization);
		}

		void prepare() {
			loader.prepare(preparation);
		}

		void loadMonoFrames(vector_type::size_type n) {
			left.resize(n);
			std::vector<gsl::span<float>> mono{ left };
			loader.load(mono);
		}

		void loadStereoFrames(vector_type::size_type n) {
			left.resize(n);
			right.resize(n);
			std::vector<gsl::span<float>> stereo{ left, right };
			loader.load(stereo);
		}

		template<typename T>
		T rms(std::vector<T> x) {
			return std::sqrt(
				std::accumulate(
					x.begin(),
					x.end(),
					T{ 0 },
					[](T a, T b) { return a += b * b; }
				) / x.size()
			);
		}
	};

	TEST_F(AudioProcessingLoaderTests, initializePassesParametersToFactoryForExceptionCheck) {
		GlobalTestParameters x;
		initialization.global = &x;
		initialize();

		EXPECT_EQ(&x, processorFactory.parameters().global);
		
		// The SpatializedHearingAidSimulationFactory expects a size of two.
		// Ideally this would not cater to a single implementation...
		EXPECT_EQ(2U, processorFactory.parameters().channelScalars.size());
	}

	TEST_F(AudioProcessingLoaderTests, queriesDoNotThrowIfNotPrepared) {
		EXPECT_EQ(0, loader.channels());
		EXPECT_EQ(0, loader.sampleRate());
		EXPECT_EQ(0, loader.chunkSize());
		EXPECT_TRUE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, preferredBufferSizeReturnsThatOfFactory) {
		processorFactory.setPreferredBufferSize(1);
		EXPECT_EQ(1, loader.preferredBufferSize());
	}

	TEST_F(AudioProcessingLoaderTests, preparePassesAllParametersToFactories) {
		GlobalTestParameters x;
		initialization.global = &x;
		initialize();
		preparation.audioFilePath = "d";
		reader->setChannels(6);
		reader->setSampleRate(7);
		prepare();
		assertEqual("d", readerFactory.filePath());
		EXPECT_EQ(6, processorFactory.parameters().channels);
		EXPECT_EQ(7, processorFactory.parameters().sampleRate);
		EXPECT_EQ(&x, processorFactory.parameters().global);
	}

	TEST_F(AudioProcessingLoaderTests, preparePassesCalibrationScaleToProcessorFactory) {
		//initialization.max_dB_Spl = 8;
		initialize();
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fakeReader));
		preparation.level_dB_Spl = 7;
		prepare();
		auto desiredRms = std::pow(10.0, (7 - 8) / 20.0);
		assertEqual(
			{
				desiredRms / rms<float>({ 1, 3, 5 }),
				desiredRms / rms<float>({ 2, 4, 6 })
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

	TEST_F(AudioProcessingLoaderTests, loadPadsZeroToEndOfReadInput) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fakeReader));
		prepare();
		loadMonoFrames(4);
		assertEqual({ 1, 2, 3, 0 }, left);
	}

	TEST_F(AudioProcessingLoaderTests, loadPadsZeroToEndOfStereoInput) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fakeReader));
		prepare();
		loadStereoFrames(4);
		assertEqual({ 1, 3, 5, 0 }, left);
		assertEqual({ 2, 4, 6, 0 }, right);
	}

	TEST_F(AudioProcessingLoaderTests, completeAfterLoadingGroupDelayManyZeros) {
		prepare();
		processor->setGroupDelay(3);
		loadMonoFrames(1);
		EXPECT_FALSE(loader.complete());
		loadMonoFrames(1);
		EXPECT_FALSE(loader.complete());
		loadMonoFrames(1);
		EXPECT_TRUE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, preparationResetsZeroPadCount) {
		prepare();
		processor->setGroupDelay(1);
		loadMonoFrames(1);
		EXPECT_TRUE(loader.complete());
		prepare();
		EXPECT_FALSE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, completeAfterLoadingGroupDelayManyZerosWithPartiallyPaddedLoad) {
		FakeAudioFileReader fakeReader{ { 0 } };
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fakeReader));
		prepare();
		processor->setGroupDelay(2);
		loadMonoFrames(2);
		EXPECT_FALSE(loader.complete());
		loadMonoFrames(2);
		EXPECT_TRUE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, preferredProcessingSizesReturnsThatOfProcessorFactory) {
		processorFactory.setPreferredProcessingSizes({ 1, 2, 3 });
		assertEqual({ 1, 2, 3 }, loader.preferredProcessingSizes());
	}

	class TimesTwo : public AudioFrameProcessor {
		void process(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x *= 2;
		}

		channel_type::index_type groupDelay() override { return {}; }
	};

	TEST_F(AudioProcessingLoaderTests, loadReadsThenProcesses) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fakeReader));
		processorFactory.setProcessor(std::make_shared<TimesTwo>());
		prepare();
		loadMonoFrames(3);
		assertEqual({ 2, 4, 6 }, left);
	}

	class AddsOne : public AudioFrameProcessor {
		void process(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x += 1;
		}

		channel_type::index_type groupDelay() override { return {}; }
	};

	TEST_F(AudioProcessingLoaderTests, loadPadsZerosBeforeProcessing) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		readerFactory.setReader(std::make_shared<AudioFileInMemory>(fakeReader));
		processorFactory.setProcessor(std::make_shared<AddsOne>());
		prepare();
		loadMonoFrames(4);
		assertEqual({ 2, 3, 4, 1 }, left);
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