#include "Chapro.h"
#include "FltkWindow.h"
#include "LibsndfileReader.h"
#include "PortAudioDevice.h"
#include <presentation/SpatializedHearingAidSimulationPresenter.h>
#include <playing-audio/PlayAudioModel.h>

class NotRealParserFactory : public ConfigurationFileParserFactory {
	std::shared_ptr<ConfigurationFileParser> make(std::string filePath) override {
		return {};
	}
};

int main() {
	SpatializedHearingAidSimulationPresenter presenter{
		std::make_shared<PlayAudioModel>(
			std::make_shared<PortAudioDeviceFactory>(),
			std::make_shared<ChaproFactory>(),
			std::make_shared<LibsndfileReaderFactory>(),
			std::make_shared< NotRealParserFactory>()
		),
		std::make_shared<FltkWindow>()
	};
	presenter.loop();
}
