#include "Chapro.h"
#include "FltkView.h"
#include "Libsndfile.h"
#include "PortAudioDevice.h"
#include "NlohmannJsonParser.h"
#include "WindowsDirectoryReader.h"
#include "FileSystemWriter.h"
#include "MersenneTwisterRandomizer.h"
#include <audio-file-reading-writing/AudioFileWriterAdapter.h>
#include <audio-file-reading-writing/AudioFileInMemory.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <dsl-prescription/PrescriptionAdapter.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>
#include <signal-processing/ScalingProcessor.h>
#include <presentation/Presenter.h>
#include <playing-audio/AudioDevicePlayer.h>
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>
#include <test-documenting/TestDocumenter.h>
#include <spatialized-hearing-aid-simulation/ZeroPaddedLoader.h>
#include <spatialized-hearing-aid-simulation/ChannelCopier.h>
#include <spatialized-hearing-aid-simulation/SpatializedHearingAidSimulationFactory.h>
#include <spatialized-hearing-aid-simulation/CalibrationComputer.h>
#include <spatialized-hearing-aid-simulation/SpatialHearingAidModel.h>

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
	FileFilterDecoratorFactory fileDecorator{&directoryReaderFactory, ".wav"};
	MersenneTwisterRandomizer randomizer{};
	RandomizedStimulusList stimulusList{&fileDecorator, &randomizer};
	FileSystemWriter persistentWriter;
	TestDocumenter testDocumenter{ &persistentWriter };
	PortAudioDevice audioDevice{};
	AudioDevicePlayer player{&audioDevice};
	ZeroPaddedLoaderFactory audioLoaderFactory{};
	LibsndfileFactory audioFileFactory{};
	AudioFileInMemoryFactory inMemoryFactory{&audioFileFactory};
	ChannelCopierFactory audioFrameReaderFactory{ &inMemoryFactory };
	AudioFileWriterAdapterFactory audioFrameWriterFactory{ &audioFileFactory };
	NlohmannJsonParserFactory parserFactory{};
	PrescriptionAdapter prescriptionReader{ &parserFactory };
	BrirAdapter brirReader{ &audioFileFactory };
	ScalarFactoryImpl scalarFactory{};
	ChaproFactory compressorFactory{};
	FirFilterFactoryImpl firFilterFactory{};
	HearingAidFactoryImpl hearingAidFactory{&compressorFactory};
	SpatializedHearingAidSimulationFactory simulationFactory{
		&scalarFactory, 
		&firFilterFactory, 
		&hearingAidFactory
	};
	CalibrationComputerFactoryImpl calibrationComputerFactory{};
	SpatialHearingAidModel model{
		&stimulusList,
		&testDocumenter,
		&player,
		&audioLoaderFactory,
		&audioFrameReaderFactory,
		&audioFrameWriterFactory,
		&prescriptionReader, 
		&brirReader, 
		&simulationFactory,
		&calibrationComputerFactory
	};
	FltkView view{};
	Presenter presenter{ &model, &view };
	presenter.run();
}
