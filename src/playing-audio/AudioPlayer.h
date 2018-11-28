#pragma once

#include <common-includes/Interface.h>

class AudioPlayer {
public:
	INTERFACE_OPERATIONS(AudioPlayer);
	virtual void play() = 0;
};
