#pragma once

#include <common-includes/Interface.h>
#include <string>

class PersistentMemoryWriter {
public:
	INTERFACE_OPERATIONS(PersistentMemoryWriter);
	virtual void write(std::string) = 0;
};

#include <presentation/Presenter.h>

#ifdef SPATIALIZED_HA_SIMULATION_UTILITY_EXPORTS
	#define SPATIALIZED_HA_SIMULATION_UTILITY_API __declspec(dllexport)
#else
	#define SPATIALIZED_HA_SIMULATION_UTILITY_API __declspec(dllimport)
#endif

class SpatializedHearingAidSimulationTestDocumenter {
	PersistentMemoryWriter *writer;
public:
	SPATIALIZED_HA_SIMULATION_UTILITY_API explicit 
		SpatializedHearingAidSimulationTestDocumenter(PersistentMemoryWriter *);
	SPATIALIZED_HA_SIMULATION_UTILITY_API void documentTestParameters(GlobalTestParameters *);
};

