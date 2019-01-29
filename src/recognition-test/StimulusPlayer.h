#pragma once

#include <common-includes/Interface.h>

class StimulusPlayer {
public:
	INTERFACE_OPERATIONS(StimulusPlayer);

	virtual void play() = 0;
	virtual bool isPlaying() = 0;
	virtual void stop() = 0;
};