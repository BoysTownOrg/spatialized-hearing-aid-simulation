#pragma once

#include <common-includes/Interface.h>
#include <string>

class PersistentMemoryWriter {
public:
	INTERFACE_OPERATIONS(PersistentMemoryWriter);
	virtual void write(std::string) = 0;
};

#include <presentation/Presenter.h>

class SpatializedHearingAidSimulationTestDocumenter {
	PersistentMemoryWriter *writer;
public:
	PRESENTATION_API explicit SpatializedHearingAidSimulationTestDocumenter(PersistentMemoryWriter *);
	PRESENTATION_API void documentTestParameters(GlobalTestParameters *);
};

