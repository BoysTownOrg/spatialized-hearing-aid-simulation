#pragma once

#include <common-includes/Interface.h>
#include <string>

class PersistentMemoryWriter {
public:
    INTERFACE_OPERATIONS(PersistentMemoryWriter)
	virtual void write(std::string) = 0;
	virtual void initialize(std::string) = 0;
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
};

