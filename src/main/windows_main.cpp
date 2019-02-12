#include "Chapro.h"
#include "FltkView.h"
#include "LibsndfileReader.h"
#include "PortAudioDevice.h"
#include "NlohmannJsonParser.h"
#include "WindowsDirectoryReader.h"
#include "FileSystemWriter.h"
#include "MersenneTwisterRandomizer.h"
#include <audio-file-reading/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <dsl-prescription/PrescriptionAdapter.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>
#include <signal-processing/ScalingProcessor.h>
#include <presentation/Presenter.h>
#include <playing-audio/AudioPlayer.h>
#include <playing-audio/AudioProcessingLoader.h>
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>
#include <test-documenting/TestDocumenter.h>
#include <spatialized-hearing-aid-simulation/SpatializedHearingAidSimulationFactory.h>
#include <spatialized-hearing-aid-simulation/CalibrationComputer.h>
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

class CalibrationComputerFactoryImpl : public ICalibrationComputerFactory {
	std::shared_ptr<ICalibrationComputer> make(AudioFrameReader *r) override {
		return std::make_shared<CalibrationComputer>(*r);
	}
};

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
	WindowsDirectoryReaderFactory directoryReaderFactory{};
	FileFilterDecorator fileDecorator{&directoryReader, ".wav"};
	MersenneTwisterRandomizer randomizer{};
	RandomizedStimulusList stimulusList{&directoryReaderFactory, &randomizer};
	FileSystemWriter persistentWriter;
	TestDocumenter testDocumenter{ &persistentWriter };
	PortAudioDevice audioDevice{};
	AudioPlayer player{&audioDevice};
	AudioProcessingLoader audioLoader{};
	LibsndfileReaderFactory audioFileReaderFactory{};
	AudioFileInMemoryFactory inMemoryFactory{&audioFileReaderFactory};
	ChannelCopierFactory audioFrameReaderFactory{ &inMemoryFactory };
	NlohmannJsonParserFactory parserFactory{};
	PrescriptionAdapter prescriptionReader{ &parserFactory };
	BrirAdapter brirReader{ &audioFileReaderFactory };
	ScalarFactoryImpl scalarFactory{};
	ChaproFactory compressorFactory{};
	FirFilterFactoryImpl firFilterFactory{};
	HearingAidFactoryImpl hearingAidFactory{&compressorFactory};
	SpatializedHearingAidSimulationFactory simulationFactory{
		&scalarFactory, 
		&firFilterFactory, 
		&hearingAidFactory
	};
	CalibrationComputerFactoryImpl calibrationFactory{};
	RefactoredModel model{
		&stimulusList,
		&testDocumenter,
		&player,
		&audioLoader,
		&audioFrameReaderFactory,
		&prescriptionReader, 
		&brirReader, 
		&simulationFactory,
		&calibrationFactory
	};
	FltkView view{};
	Presenter presenter{ &model, &view };
	presenter.run();
}
