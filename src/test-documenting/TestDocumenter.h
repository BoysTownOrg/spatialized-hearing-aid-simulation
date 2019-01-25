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
		void documentTestParameters(TestParameters) override;
	TEST_DOCUMENTING_API 
		void documentTrialParameters(TrialParameters) override;
	TEST_DOCUMENTING_API 
		void initialize(std::string filePath) override;
};

