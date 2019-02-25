#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFile.h"
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <playing-audio/ZeroPaddedLoader.h>
#include <gtest/gtest.h>

namespace {
	class ZeroPaddedLoaderTests : public ::testing::Test {
	protected:
		using channel_type = ZeroPaddedLoader::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;
		using size_type = buffer_type::size_type;
		buffer_type left{};
		buffer_type right{};
		std::shared_ptr<AudioFrameReaderStub> reader =
			std::make_shared<AudioFrameReaderStub>();
		std::shared_ptr<AudioFrameProcessorStub> processor =
			std::make_shared<AudioFrameProcessorStub>();
		ZeroPaddedLoader loader{reader, processor};

		void reset() {
			loader.reset();
		}

		void loadMonoFrames(size_type n) {
			left.resize(n);
			std::vector<channel_type> mono{ left };
			loader.load(mono);
		}

		void loadStereoFrames(size_type n) {
			left.resize(n);
			right.resize(n);
			std::vector<channel_type> stereo{ left, right };
			loader.load(stereo);
		}

		void reconstructInMemory(
			AudioFileReader &reader_, 
			std::shared_ptr<AudioFrameProcessor> p =
				std::make_shared<AudioFrameProcessorStub>()
		) {
			loader = { std::make_shared<AudioFileInMemory>(reader_), std::move(p) };
		}

		void assertComplete() {
			assertTrue(loader.complete());
		}

		void assertIncomplete() {
			assertFalse(loader.complete());
		}
	};

	TEST_F(ZeroPaddedLoaderTests, queriesDoNotThrow) {
		assertTrue(loader.complete());
	}

	TEST_F(ZeroPaddedLoaderTests, loadPadsZeroToEndOfInput_Mono) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		reconstructInMemory(fakeReader);
		loadMonoFrames(4);
		assertEqual({ 1, 2, 3, 0 }, left);
	}

	TEST_F(ZeroPaddedLoaderTests, loadPadsZeroToEndOfInput_Stereo) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		reconstructInMemory(fakeReader);
		loadStereoFrames(4);
		assertEqual({ 1, 3, 5, 0 }, left);
		assertEqual({ 2, 4, 6, 0 }, right);
	}

	TEST_F(ZeroPaddedLoaderTests, completeAfterLoadingGroupDelayManyZeros) {
		processor->setGroupDelay(3);
		loadMonoFrames(1);
		assertIncomplete();
		loadMonoFrames(1);
		assertIncomplete();
		loadMonoFrames(1);
		assertComplete();
	}

	TEST_F(ZeroPaddedLoaderTests, completeAfterLoadingGroupDelayManyZeros_PartiallyPaddedLoad) {
		buffer_type tenSamples(10);
		FakeAudioFileReader fakeReader{ tenSamples };
		reconstructInMemory(fakeReader, processor);
		processor->setGroupDelay(2);
		loadMonoFrames(10 + 1);
		assertIncomplete();
		loadMonoFrames(1);
		assertComplete();
	}

	TEST_F(ZeroPaddedLoaderTests, notCompleteIfReaderStillHasFramesRemaining) {
		reader->setRemainingFrames(1);
		assertIncomplete();
	}

	TEST_F(ZeroPaddedLoaderTests, resetResetsZeroPadCount) {
		processor->setGroupDelay(1);
		loadMonoFrames(1);
		assertComplete();
		reset();
		assertIncomplete();
	}

	class TimesTwo : public AudioFrameProcessor {
		void process(gsl::span<channel_type> audio) override {
			for (auto channel : audio)
				for (auto &x : channel)
					x *= 2;
		}

		channel_type::index_type groupDelay() override { return {}; }
	};

	TEST_F(ZeroPaddedLoaderTests, loadReadsThenProcesses) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		reconstructInMemory(fakeReader, std::make_shared<TimesTwo>());
		loadMonoFrames(3);
		assertEqual({ 2, 4, 6 }, left);
	}

	class AddsOne : public AudioFrameProcessor {
		void process(gsl::span<channel_type> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x += 1;
		}

		channel_type::index_type groupDelay() override { return {}; }
	};

	TEST_F(ZeroPaddedLoaderTests, loadPadsZerosBeforeProcessing) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		reconstructInMemory(fakeReader, std::make_shared<AddsOne>());
		loadMonoFrames(4);
		assertEqual({ 1 + 1, 2 + 1, 3 + 1, 0 + 1 }, left);
	}
}