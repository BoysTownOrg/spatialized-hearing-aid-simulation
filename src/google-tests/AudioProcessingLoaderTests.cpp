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
		using impulse_response_type = std::vector<float>;
		impulse_response_type left{};
		impulse_response_type right{};
		std::shared_ptr<AudioFrameReaderStub> reader =
			std::make_shared<AudioFrameReaderStub>();
		std::shared_ptr<AudioFrameProcessorStub> processor =
			std::make_shared<AudioFrameProcessorStub>();
		AudioProcessingLoader loader{};

		AudioProcessingLoaderTests() {
			setReader(reader);
			setProcessor(processor);
		}

		void setReader(std::shared_ptr<AudioFrameReader> r) {
			loader.setReader(std::move(r));
		}

		void setProcessor(std::shared_ptr<AudioFrameProcessor> p) {
			loader.setProcessor(std::move(p));
		}

		void reset() {
			loader.reset();
		}

		void loadMonoFrames(impulse_response_type::size_type n) {
			left.resize(n);
			std::vector<gsl::span<float>> mono{ left };
			loader.load(mono);
		}

		void loadStereoFrames(impulse_response_type::size_type n) {
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

		void setInMemoryReader(AudioFileReader &reader_) {
			setReader(std::make_shared<AudioFileInMemory>(reader_));
		}
	};

	TEST_F(AudioProcessingLoaderTests, queriesDoNotThrow) {
		EXPECT_TRUE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, DISABLED_preparePassesCalibratedScalarsToProcessorFactory) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		setInMemoryReader(fakeReader);
		//preparation.level_dB_Spl = 7;
		//processorFactory.setFullScale_dB_Spl(8);
		reset();
//		auto desiredRms = std::pow(10.0, (7 - 8) / 20.0);
		/*assertEqual(
			{
				desiredRms / rms<float>({ 1, 3, 5 }),
				desiredRms / rms<float>({ 2, 4, 6 })
			},
			processorFactory.parameters().channelScalars,
			1e-6
		);*/
	}

	TEST_F(AudioProcessingLoaderTests, DISABLED_prepareResetsReaderAfterComputingRms) {
		reset();
		EXPECT_TRUE(reader->readingLog().endsWith("reset "));
	}

	TEST_F(AudioProcessingLoaderTests, loadPadsZeroToEndOfReadInput) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		setInMemoryReader(fakeReader);
		reset();
		loadMonoFrames(4);
		assertEqual({ 1, 2, 3, 0 }, left);
	}

	TEST_F(AudioProcessingLoaderTests, loadPadsZeroToEndOfStereoInput) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		setInMemoryReader(fakeReader);
		reset();
		loadStereoFrames(4);
		assertEqual({ 1, 3, 5, 0 }, left);
		assertEqual({ 2, 4, 6, 0 }, right);
	}

	TEST_F(AudioProcessingLoaderTests, completeAfterLoadingGroupDelayManyZeros) {
		reset();
		processor->setGroupDelay(3);
		loadMonoFrames(1);
		EXPECT_FALSE(loader.complete());
		loadMonoFrames(1);
		EXPECT_FALSE(loader.complete());
		loadMonoFrames(1);
		EXPECT_TRUE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, notCompleteIfReaderStillHasFramesRemaining) {
		reset();
		reader->setRemainingFrames(1);
		EXPECT_FALSE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, preparationResetsZeroPadCount) {
		reset();
		processor->setGroupDelay(1);
		loadMonoFrames(1);
		EXPECT_TRUE(loader.complete());
		reset();
		EXPECT_FALSE(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, completeAfterLoadingGroupDelayManyZerosWithPartiallyPaddedLoad) {
		std::vector<float> singleSample { 0 };
		FakeAudioFileReader fakeReader{ singleSample };
		setInMemoryReader(fakeReader);
		reset();
		processor->setGroupDelay(2);
		loadMonoFrames(2);
		EXPECT_FALSE(loader.complete());
		loadMonoFrames(2);
		EXPECT_TRUE(loader.complete());
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
		setInMemoryReader(fakeReader);
		setProcessor(std::make_shared<TimesTwo>());
		reset();
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
		setInMemoryReader(fakeReader);
		setProcessor(std::make_shared<AddsOne>());
		reset();
		loadMonoFrames(4);
		assertEqual({ 2, 3, 4, 1 }, left);
	}
}