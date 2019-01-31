#include "Chapro.h"
#include "FltkView.h"
#include "LibsndfileReader.h"
#include "PortAudioDevice.h"
#include "NlohmannJsonParser.h"
#include "WindowsDirectoryReader.h"
#include "FileSystemWriter.h"
#include "MersenneTwisterRandomizer.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <dsl-prescription/PrescriptionAdapter.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>
#include <signal-processing/ScalingProcessor.h>
#include <presentation/Presenter.h>
#include <playing-audio/AudioPlayer.h>
#include <playing-audio/AudioProcessingLoader.h>
#include <recognition-test/RecognitionTest.h>
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>
#include <test-documenting/TestDocumenter.h>
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>

class HearingAidFactoryImpl : public HearingAidFactory {
	FilterbankCompressorFactory *compressorFactory;
public:
	explicit HearingAidFactoryImpl(FilterbankCompressorFactory *compressorFactory) : 
		compressorFactory{ compressorFactory } {}

	std::shared_ptr<SignalProcessor> make(FilterbankCompressor::Parameters p) override {
		return std::make_shared<HearingAidProcessor>(compressorFactory->make(std::move(p)));
	}
};

class FirFilterFactoryImpl : public FirFilterFactory {
	std::shared_ptr<SignalProcessor> make(BrirReader::impulse_response_type b) override {
		return std::make_shared<FirFilter>(std::move(b));
	}
};

class ScalarFactoryImpl : public ScalarFactory {
	std::shared_ptr<SignalProcessor> make(float x) override {
		return std::make_shared<ScalingProcessor>(x);
	}
};

int main() {
	WindowsDirectoryReader directoryReader{};
	FileFilterDecorator fileDecorator{&directoryReader, ".wav"};
	MersenneTwisterRandomizer randomizer{};
	RandomizedStimulusList stimulusList{&fileDecorator, &randomizer};
	FileSystemWriter persistentWriter;
	TestDocumenter testDocumenter{ &persistentWriter };
	RecognitionTest perceptionTest{ &stimulusList, &testDocumenter };
	PortAudioDevice audioDevice{};
	AudioPlayer player{&audioDevice};
	AudioProcessingLoader audioLoader{};
	ChannelCopierFactory audioFrameReaderFactory{ 
		std::make_shared<AudioFileInMemoryFactory>(
			std::make_shared<LibsndfileReaderFactory>()
		) 
	};
	ChaproFactory compressorFactory{};
	HearingAidFactoryImpl hearingAidFactory{&compressorFactory};
	PrescriptionAdapter prescriptionReader{ std::make_shared<NlohmannJsonParserFactory>() };
	FirFilterFactoryImpl firFilterFactory{};
	BrirAdapter brirReader{ std::make_shared<LibsndfileReaderFactory>() };
	ScalarFactoryImpl scalarFactory{};
	RefactoredModel model{
		&perceptionTest,
		&player,
		&audioLoader,
		&audioFrameReaderFactory,
		&hearingAidFactory, 
		&prescriptionReader, 
		&firFilterFactory,
		&brirReader, 
		&scalarFactory
	};
	FltkView view{};
	Presenter presenter{ &model, &view };
	presenter.run();
}
