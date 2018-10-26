#pragma once

#include <common-includes/Interface.h>

class MonoProcessor {
public:
	INTERFACE_OPERATIONS(MonoProcessor);
	virtual void process(float *, int) = 0;
};
