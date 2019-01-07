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
#include <playing-audio/SpatializedHearingAidSimulationFactory.h>
#include <playing-audio/PlayAudioModel.h>

int main() {
	Presenter presenter{
		std::make_shared<PlayAudioModel>(
			std::make_shared<PortAudioDevice>(),
			std::make_shared<ChannelCopierFactory>(
				std::make_shared<AudioFileInMemoryFactory>(
					std::make_shared<LibsndfileReaderFactory>()
				)
			),
			std::make_shared<SpatializedHearingAidSimulationFactory>(
				std::make_shared<ChaproFactory>(),
				std::make_shared<PrescriptionAdapter>(
					std::make_shared<NlohmannJsonParserFactory>()
				),
				std::make_shared<BrirAdapter>(
					std::make_shared<LibsndfileReaderFactory>()
				)
			)
		),
		std::make_shared<FltkWindow>()
	};
	presenter.run();
}
