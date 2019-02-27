#pragma once

#include "PersistentMemoryWriter.h"
#include <spatialized-hearing-aid-simulation/Documenter.h>

#ifdef TEST_DOCUMENTING_EXPORTS
	#define TEST_DOCUMENTING_API __declspec(dllexport)
#else
	#define TEST_DOCUMENTING_API __declspec(dllimport)
#endif

class TestDocumenter : public Documenter {
	PersistentMemoryWriter *writer;
public:
	TEST_DOCUMENTING_API explicit 
		TestDocumenter(PersistentMemoryWriter *);
	TEST_DOCUMENTING_API 
		void documentTestParameters(Model::Testing *) override;
	TEST_DOCUMENTING_API 
		void documentTrialParameters(TrialParameters) override;
	TEST_DOCUMENTING_API 
		void initialize(std::string filePath) override;
};

