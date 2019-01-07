#pragma once

#include <common-includes/Interface.h>

class EventListener {
public:
	INTERFACE_OPERATIONS(EventListener);
	virtual void browseForLeftDslPrescription() = 0;
	virtual void browseForRightDslPrescription() = 0;
	virtual void browseForAudio() = 0;
	virtual void browseForBrir() = 0;
	virtual void play() = 0;
};