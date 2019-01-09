#include "Chapro.h"
#include "FltkWindow.h"
#include "LibsndfileReader.h"
#include "PortAudioDevice.h"
#include "NlohmannJsonParser.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <dsl-prescription/PrescriptionAdapter.h>
#include <presentation/Presenter.h>
#include <playing-audio/AudioPlayer.h>
#include <playing-audio/SpatializedHearingAidSimulationFactory.h>
#include <recognition-test/RecognitionTestModel.h>
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>

class WindowsDirectoryReader : public DirectoryReader {
	std::vector<std::string> filesIn(std::string directory) override {
		return std::vector<std::string>();
	}
};

#include <random>

class MersenneTwisterRandomizer : public Randomizer {
    std::mt19937 engine{std::random_device{}()};
public:
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }
};

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
	AudioPlayer player{&device, &frameReaderFactory, &processorFactory};
	RecognitionTestModel model{ &list, &player };
	FltkWindow view{};
	Presenter presenter{ &model, &view };
	presenter.run();
}
