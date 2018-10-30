#pragma once

#include "presentation-exports.h"
#include "Model.h"

class AudioPlayerModel : public Model {
public:
	PRESENTATION_API void playRequest(PlayRequest) override;
private:
	void throwIfNotDouble(std::string x, std::string identifier);
	void throwIfNotPositiveInteger(std::string x, std::string identifier);
	void throwRequestFailure(std::string x, std::string identifier);
};
