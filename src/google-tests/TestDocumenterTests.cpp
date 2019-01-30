#include "assert-utility.h"
#include "PersistentMemoryWriterStub.h"
#include <test-documenting/TestDocumenter.h>
#include <gtest/gtest.h>

class TestDocumenterTests : public ::testing::Test {
protected:
	PersistentMemoryWriterStub writer;
	TestDocumenter documenter{ &writer };
};

TEST_F(
	TestDocumenterTests,
	DISABLED_formatsTestParameters
) {
	TestDocumenter::TestParameters test;
	//perceptionTest.subjectId = "a";
	//perceptionTest.testerId = "b";
	//perceptionTest.usingHearingAidSimulation = true;
	//perceptionTest.leftDslPrescriptionFilePath = "c";
	//perceptionTest.rightDslPrescriptionFilePath = "d";
	//perceptionTest.usingSpatialization = true;
	//perceptionTest.brirFilePath = "e";
	//perceptionTest.attack_ms = 1;
	//perceptionTest.release_ms = 2;
	//perceptionTest.windowSize = 3;
	//perceptionTest.chunkSize = 4;
	documenter.documentTestParameters(test);
	assertEqual(
		"subject: a\n"
		"tester: b\n"
		"DSL prescription\n"
		"    left: c\n"
		"    right: d\n"
		"BRIR: e\n"
		"attack (ms): 1.0\n"
		"release (ms): 2.0\n"
		"window size (samples): 3\n"
		"chunk size (samples): 4\n\n", 
		writer.content()
	);
}

TEST_F(
	TestDocumenterTests,
	DISABLED_ignoresPrescriptionsIfNotUsingHearingAidSimulation
) {
	TestDocumenter::TestParameters test;
	//perceptionTest.subjectId = "a";
	//perceptionTest.testerId = "b";
	//perceptionTest.usingHearingAidSimulation = false;
	//perceptionTest.leftDslPrescriptionFilePath = "c";
	//perceptionTest.rightDslPrescriptionFilePath = "d";
	//perceptionTest.usingSpatialization = true;
	//perceptionTest.brirFilePath = "e";
	//perceptionTest.attack_ms = 1;
	//perceptionTest.release_ms = 2;
	//perceptionTest.windowSize = 3;
	//perceptionTest.chunkSize = 4;
	TestDocumenter::TestParameters p;
	documenter.documentTestParameters(p);
	assertEqual(
		"subject: a\n"
		"tester: b\n"
		"BRIR: e\n"
		"attack (ms): 1.0\n"
		"release (ms): 2.0\n"
		"window size (samples): 3\n"
		"chunk size (samples): 4\n\n", 
		writer.content()
	);
}

TEST_F(
	TestDocumenterTests,
	DISABLED_ignoresBrirIfNotUsingSpatialization
) {
	TestDocumenter::TestParameters test;
	//perceptionTest.subjectId = "a";
	//perceptionTest.testerId = "b";
	//perceptionTest.usingHearingAidSimulation = true;
	//perceptionTest.leftDslPrescriptionFilePath = "c";
	//perceptionTest.rightDslPrescriptionFilePath = "d";
	//perceptionTest.usingSpatialization = false;
	//perceptionTest.brirFilePath = "e";
	//perceptionTest.attack_ms = 1;
	//perceptionTest.release_ms = 2;
	//perceptionTest.windowSize = 3;
	//perceptionTest.chunkSize = 4;
	TestDocumenter::TestParameters p;
	documenter.documentTestParameters(p);
	assertEqual(
		"subject: a\n"
		"tester: b\n"
		"DSL prescription\n"
		"    left: c\n"
		"    right: d\n"
		"attack (ms): 1.0\n"
		"release (ms): 2.0\n"
		"window size (samples): 3\n"
		"chunk size (samples): 4\n\n", 
		writer.content()
	);
}

TEST_F(
	TestDocumenterTests,
	DISABLED_playTrialDocumentsTrial
) {
	TestDocumenter::TrialParameters trial;
	//trial.level_dB_Spl = 1;
	trial.stimulus = "a";
	documenter.documentTrialParameters(trial);
	assertEqual(
		"stimulus: a\n"
		"level (dB SPL): 1\n\n", 
		writer.content()
	);
}

TEST_F(
	TestDocumenterTests,
	initializePassesFilePath
) {
	documenter.initialize("a");
	assertEqual("a", writer.filePath());
}

TEST(
	TestDocumenterWithInitializationFailingWriter,
	initializeTestThrowsInitializationFailureWhenDocumenterFailsToInitialize
) {
	InitializationFailingWriter writer{};
	writer.setErrorMessage("error.");
	TestDocumenter documenter{ &writer };
	try {
		documenter.initialize({});
		FAIL() << "Expected TestDocumenter::InitializationFailure";
	}
	catch (const TestDocumenter::InitializationFailure &e) {
		assertEqual("error.", e.what());
	}
}