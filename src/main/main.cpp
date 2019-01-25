#include "Chapro.h"
#include "FltkView.h"
#include "LibsndfileReader.h"
#include "PortAudioDevice.h"
#include "NlohmannJsonParser.h"
#include "WindowsDirectoryReader.h"
#include "FileSystemWriter.h"
#include "MersenneTwisterRandomizer.h"
#include "SpatializedHearingAidSimulationFactory.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <dsl-prescription/PrescriptionAdapter.h>
#include <presentation/Presenter.h>
#include <playing-audio/AudioPlayer.h>
#include <playing-audio/AudioProcessingLoader.h>
#include <recognition-test/RecognitionTestModel.h>
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>
#include <spatialized-hearing-aid-simulation-utility/SpatializedHearingAidSimulationTestDocumenter.h>

int main() {
	WindowsDirectoryReader reader{};
	FileFilterDecorator decorator{&reader, ".wav"};
	MersenneTwisterRandomizer randomizer{};
	RandomizedStimulusList list{&decorator, &randomizer};
	PortAudioDevice device{};
	ChannelCopierFactory frameReaderFactory{ 
		std::make_shared<AudioFileInMemoryFactory>(
			std::make_shared<LibsndfileReaderFactory>()
		) 
	};
	SpatializedHearingAidSimulationFactory processorFactory{
		std::make_shared<ChaproFactory>(),
		std::make_shared<PrescriptionAdapter>(
			std::make_shared<NlohmannJsonParserFactory>()
		),
		std::make_shared<BrirAdapter>(
			std::make_shared<LibsndfileReaderFactory>()
		)
	};
	AudioProcessingLoader loader{&frameReaderFactory, &processorFactory};
	AudioPlayer player{&device, &loader};
	FileSystemWriter writer;
	TestDocumenter documenter{ &writer };
	RecognitionTestModel model{ &list, &player, &documenter };
	FltkView view{};
	Presenter presenter{ &model, &view };
	presenter.run();
}
