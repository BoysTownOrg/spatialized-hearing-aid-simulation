#pragma once

#include <common-includes/Interface.h>
#include <string>

class PersistentMemoryWriter {
public:
	INTERFACE_OPERATIONS(PersistentMemoryWriter);
	virtual void write(std::string) = 0;
	virtual void initialize(std::string) = 0;
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
};

#include <presentation/Presenter.h>
#include <recognition-test/RecognitionTestModel.h>
#include <common-includes/RuntimeError.h>

#ifdef SPATIALIZED_HA_SIMULATION_UTILITY_EXPORTS
	#define SPATIALIZED_HA_SIMULATION_UTILITY_API __declspec(dllexport)
#else
	#define SPATIALIZED_HA_SIMULATION_UTILITY_API __declspec(dllimport)
#endif

class SpatializedHearingAidSimulationTestDocumenter : public Documenter {
	PersistentMemoryWriter *writer;
public:
	SPATIALIZED_HA_SIMULATION_UTILITY_API explicit 
		SpatializedHearingAidSimulationTestDocumenter(PersistentMemoryWriter *);
	SPATIALIZED_HA_SIMULATION_UTILITY_API void documentTestParameters(TestParameters) override;
	SPATIALIZED_HA_SIMULATION_UTILITY_API void documentTrialParameters(TrialParameters) override;
	SPATIALIZED_HA_SIMULATION_UTILITY_API void initialize(std::string filePath) override;
};

