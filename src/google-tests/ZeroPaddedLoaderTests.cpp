#include "assert-utility.h"
#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include "FakeAudioFile.h"
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <spatialized-hearing-aid-simulation/ZeroPaddedLoader.h>
#include <gtest/gtest.h>

namespace {
	class ZeroPaddedLoaderFacade {
		ZeroPaddedLoader loader;
	public:
		using channel_type = ZeroPaddedLoader::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;
		using size_type = buffer_type::size_type;
		buffer_type left{};
		buffer_type right{};

		ZeroPaddedLoaderFacade(
			AudioFileReader &reader,
			std::shared_ptr<AudioFrameProcessor> processor
		) :
			loader{ std::make_shared<AudioFileInMemory>(reader), std::move(processor) } {}

		ZeroPaddedLoaderFacade(
			std::shared_ptr<AudioFrameReader> r,
			std::shared_ptr<AudioFrameProcessor> p
		) :
			loader{ std::move(r), std::move(p) } {}

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

		auto complete() {
			return loader.complete();
		}

		auto reset() {
			return loader.reset();
		}
	};

	class ZeroPaddedLoaderTests : public ::testing::Test {
	protected:
		std::shared_ptr<AudioFrameReaderStub> reader =
			std::make_shared<AudioFrameReaderStub>();
		std::shared_ptr<AudioFrameProcessorStub> processor =
			std::make_shared<AudioFrameProcessorStub>();

		ZeroPaddedLoaderFacade construct(
			AudioFileReader &reader_, 
			std::shared_ptr<AudioFrameProcessor> p =
				std::make_shared<AudioFrameProcessorStub>()
		) {
			return ZeroPaddedLoaderFacade{ reader_, std::move(p) };
		}

		ZeroPaddedLoaderFacade construct() {
			return ZeroPaddedLoaderFacade{ reader, processor };
		}
	};

	TEST_F(ZeroPaddedLoaderTests, queriesDoNotThrow) {
		auto loader = construct();
		assertTrue(loader.complete());
	}

	TEST_F(ZeroPaddedLoaderTests, loadPadsZeroToEndOfInputMono) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		auto loader = construct(fakeReader);
		loader.loadMonoFrames(4);
		assertEqual({ 1, 2, 3, 0 }, loader.left);
	}

	TEST_F(ZeroPaddedLoaderTests, loadPadsZeroToEndOfInputStereo) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3, 4, 5, 6 } };
		fakeReader.setChannels(2);
		auto loader = construct(fakeReader);
		loader.loadStereoFrames(4);
		assertEqual({ 1, 3, 5, 0 }, loader.left);
		assertEqual({ 2, 4, 6, 0 }, loader.right);
	}

	TEST_F(ZeroPaddedLoaderTests, completeAfterLoadingGroupDelayManyZeros) {
		processor->setGroupDelay(3);
		auto loader = construct();
		loader.loadMonoFrames(1);
		assertFalse(loader.complete());
		loader.loadMonoFrames(1);
		assertFalse(loader.complete());
		loader.loadMonoFrames(1);
		assertTrue(loader.complete());
	}

	TEST_F(ZeroPaddedLoaderTests, completeAfterLoadingGroupDelayManyZerosPartiallyPaddedLoad) {
		ZeroPaddedLoaderFacade::buffer_type tenSamples(10);
		FakeAudioFileReader fakeReader{ tenSamples };
		auto loader = construct(fakeReader, processor);
		processor->setGroupDelay(2);
		loader.loadMonoFrames(10 + 1);
		assertFalse(loader.complete());
		loader.loadMonoFrames(1);
		assertTrue(loader.complete());
	}

	TEST_F(ZeroPaddedLoaderTests, notCompleteIfReaderStillHasFramesRemaining) {
		reader->setRemainingFrames(1);
		auto loader = construct();
		assertFalse(loader.complete());
	}

	TEST_F(ZeroPaddedLoaderTests, resetResetsZeroPadCount) {
		processor->setGroupDelay(1);
		auto loader = construct();
		loader.loadMonoFrames(1);
		assertTrue(loader.complete());
		loader.reset();
		assertFalse(loader.complete());
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
		auto loader = construct(fakeReader, std::make_shared<TimesTwo>());
		loader.loadMonoFrames(3);
		assertEqual({ 2, 4, 6 }, loader.left);
	}

	class AddsOne : public AudioFrameProcessor {
		void process(gsl::span<channel_type> audio) override {
			for (const auto &channel : audio)
				for (auto &x : channel)
					x += 1;
		}

		channel_type::index_type groupDelay() override { return {}; }
	};

	TEST_F(ZeroPaddedLoaderTests, loadPadsZerosBeforeProcessing) {
		FakeAudioFileReader fakeReader{ { 1, 2, 3 } };
		auto loader = construct(fakeReader, std::make_shared<AddsOne>());
		loader.loadMonoFrames(4);
		assertEqual({ 1 + 1, 2 + 1, 3 + 1, 0 + 1 }, loader.left);
	}
}
