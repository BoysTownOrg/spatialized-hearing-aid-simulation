#include "Chapro.h"
#include "FltkWindow.h"
#include "LibsndfileReader.h"
#include "PortAudioDevice.h"
#include <google-tests/FakeConfigurationFileParser.h>
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <presentation/Presenter.h>
#include <playing-audio/SpatializedHearingAidSimulationFactory.h>
#include <playing-audio/PlayAudioModel.h>

int main() {
	const auto parser = std::make_shared<FakeConfigurationFileParser>();
	parser->setVectorProperty(propertyName(dsl_prescription::Property::crossFrequenciesHz), 
		{
			317.16,
			502.97,
			797.63,
			1264.91,
			2005.93,
			3181.08,
			5044.66 
		}
	);
	parser->setVectorProperty(propertyName(dsl_prescription::Property::compressionRatios), 
		{
			1.1, 
			1.1, 
			1.2, 
			1.1, 
			1.5, 
			1.6, 
			1.6, 
			2.1 
		}
	);
	parser->setVectorProperty(propertyName(dsl_prescription::Property::kneepointGains_dB), 
		{
			1.92,
			1.94,
			9.94,
			1.59,
			26.72,
			35.56,
			34.20,
			48.58 
		}
	);
	parser->setVectorProperty(propertyName(dsl_prescription::Property::kneepoints_dBSpl), 
		{
			32.2,
			32.2,
			36.6,
			28.7,
			37.7,
			39.3,
			34.6,
			39.1 
		}
	);
	parser->setVectorProperty(propertyName(dsl_prescription::Property::broadbandOutputLimitingThresholds_dBSpl), 
		{
			83,
			92.05,
			95.4,
			93.87,
			104.3,
			107.45,
			102.25,
			105.1 
		}
	);
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
				std::make_shared<FakeConfigurationFileParserFactory>(parser),
				std::make_shared<BrirAdapter>(
					std::make_shared<LibsndfileReaderFactory>()
				)
			)
		),
		std::make_shared<FltkWindow>()
	};
	presenter.loop();
}
