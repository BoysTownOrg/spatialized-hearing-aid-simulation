#pragma once

#include "PersistentMemoryWriter.h"
#include <spatialized-hearing-aid-simulation/TestDocumenter.h>

#ifdef _WIN32
    #ifdef TEST_DOCUMENTING_EXPORTS
        #define TEST_DOCUMENTING_API __declspec(dllexport)
    #else
        #define TEST_DOCUMENTING_API __declspec(dllimport)
    #endif
#else
    #define TEST_DOCUMENTING_API
#endif

class TestDocumenterImpl : public TestDocumenter {
	PersistentMemoryWriter *writer;
public:
	TEST_DOCUMENTING_API explicit TestDocumenterImpl(PersistentMemoryWriter *);
	TEST_DOCUMENTING_API void documentTestParameters(const Model::Testing &) override;
	TEST_DOCUMENTING_API void documentTrialParameters(TrialParameters) override;
	TEST_DOCUMENTING_API void initialize(std::string filePath) override;
};

