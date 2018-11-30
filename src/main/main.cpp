#include "Chapro.h"
#include "FltkWindow.h"
#include "LibsndfileReader.h"
#include "PortAudioDevice.h"
#include <google-tests/MockConfigurationFileParser.h>
#include <presentation/SpatializedHearingAidSimulationPresenter.h>
#include <playing-audio/PlayAudioModel.h>

int main() {
	const auto parser = std::make_shared<MockConfigurationFileParser>();
	parser->setValidBrirProperties();
	parser->setValidSingleChannelDslProperties();
	parser->setIntProperty(propertyName(brir_config::Property::sampleRate), 44100);
	SpatializedHearingAidSimulationPresenter presenter{
		std::make_shared<PlayAudioModel>(
			std::make_shared<PortAudioDeviceFactory>(),
			std::make_shared<ChaproFactory>(),
			std::make_shared<LibsndfileReaderFactory>(),
			std::make_shared<MockParserFactory>(parser)
		),
		std::make_shared<FltkWindow>()
	};
	presenter.loop();
}
