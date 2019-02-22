#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFileReader.h"
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <playing-audio/AudioProcessingLoader.h>
#include <gtest/gtest.h>

namespace {
	class AudioProcessingLoaderTests : public ::testing::Test {
	protected:
		using channel_type = AudioProcessingLoader::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;
		using size_type = buffer_type::size_type;
		buffer_type left{};
		buffer_type right{};
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

		void setInMemoryReader(AudioFileReader &reader_) {
			setReader(std::make_shared<AudioFileInMemory>(reader_));
		}

		void assertComplete() {
			assertTrue(loader.complete());
		}

		void assertIncomplete() {
			assertFalse(loader.complete());
		}
	};

	TEST_F(AudioProcessingLoaderTests, queriesDoNotThrow) {
		assertTrue(loader.complete());
	}

	TEST_F(AudioProcessingLoaderTests, loadPadsZeroToEndOfInput_Mono) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		setInMemoryReader(fakeReader);
		loadMonoFrames(4);
		assertEqual({ 1, 2, 3, 0 }, left);
	}

	TEST_F(AudioProcessingLoaderTests, loadPadsZeroToEndOfInput_Stereo) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		setInMemoryReader(fakeReader);
		loadStereoFrames(4);
		assertEqual({ 1, 3, 5, 0 }, left);
		assertEqual({ 2, 4, 6, 0 }, right);
	}

	TEST_F(AudioProcessingLoaderTests, completeAfterLoadingGroupDelayManyZeros) {
		processor->setGroupDelay(3);
		loadMonoFrames(1);
		assertIncomplete();
		loadMonoFrames(1);
		assertIncomplete();
		loadMonoFrames(1);
		assertComplete();
	}

	TEST_F(AudioProcessingLoaderTests, completeAfterLoadingGroupDelayManyZeros_PartiallyPaddedLoad) {
		buffer_type tenSamples(10);
		FakeAudioFileReader fakeReader{ tenSamples };
		setInMemoryReader(fakeReader);
		processor->setGroupDelay(2);
		loadMonoFrames(10 + 1);
		assertIncomplete();
		loadMonoFrames(1);
		assertComplete();
	}

	TEST_F(AudioProcessingLoaderTests, notCompleteIfReaderStillHasFramesRemaining) {
		reader->setRemainingFrames(1);
		assertIncomplete();
	}

	TEST_F(AudioProcessingLoaderTests, resetResetsZeroPadCount) {
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

	TEST_F(AudioProcessingLoaderTests, loadReadsThenProcesses) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		setInMemoryReader(fakeReader);
		setProcessor(std::make_shared<TimesTwo>());
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

	TEST_F(AudioProcessingLoaderTests, loadPadsZerosBeforeProcessing) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		setInMemoryReader(fakeReader);
		setProcessor(std::make_shared<AddsOne>());
		loadMonoFrames(4);
		assertEqual({ 1 + 1, 2 + 1, 3 + 1, 0 + 1 }, left);
	}
}