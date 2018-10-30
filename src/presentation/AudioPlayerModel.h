#pragma once

#include "presentation-exports.h"
#include "Model.h"

class AudioPlayerModel : public Model {
public:
	PRESENTATION_API void playRequest(PlayRequest) override;
};

