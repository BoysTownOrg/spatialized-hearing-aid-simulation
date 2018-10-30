#pragma once

#include "presentation-exports.h"
#include "Model.h"

class AudioPlayerModel : public Model {
public:
	PRESENTATION_API void playRequest(PlayRequest) override;
private:
	void throwIfNotDouble(std::string x, std::string identifier);
	void throwIfNotInteger(std::string x, std::string identifier);
};

