#include "assert-utility.h"
#include "PersistentMemoryWriterStub.h"
#include <test-documenting/TestDocumenterImpl.h>
#include <gtest/gtest.h>

class TestDocumenterImplTests : public ::testing::Test {
protected:
	PersistentMemoryWriterStub writer;
	TestDocumenterImpl documenter{ &writer };
};

TEST_F(
	TestDocumenterImplTests,
	formatsTestParameters
) {
	Model::Testing test;
	test.subjectId = "a";
	test.testerId = "b";
	test.audioDirectory = "c";
	test.processing.usingSpatialization = true;
	test.processing.brirFilePath = "d";
	test.processing.usingHearingAidSimulation = true;
	test.processing.leftDslPrescriptionFilePath = "e";
	test.processing.rightDslPrescriptionFilePath = "f";
	test.processing.attack_ms = 1.1;
	test.processing.release_ms = 2.2;
	test.processing.windowSize = 3;
	test.processing.chunkSize = 4;
	documenter.documentTestParameters(test);
	assertEqual(
		"subject: a\n"
		"tester: b\n"
		"stimulus list: c\n"
		"\n"
		"spatialization\n"
		"    BRIR: d\n"
		"\n"
		"hearing aid simulation\n"
		"    DSL prescription\n"
		"        left: e\n"
		"        right: f\n"
		"    attack (ms): 1.1\n"
		"    release (ms): 2.2\n"
		"    window size (samples): 3\n"
		"    chunk size (samples): 4\n\n", 
		writer.content()
	);
}

TEST_F(
	TestDocumenterImplTests,
	ignoresPrescriptionsIfNotUsingHearingAidSimulation
) {
	Model::Testing test;
	test.subjectId = "a";
	test.testerId = "b";
	test.audioDirectory = "c";
	test.processing.usingSpatialization = true;
	test.processing.brirFilePath = "d";
	test.processing.usingHearingAidSimulation = false;
	documenter.documentTestParameters(test);
	assertEqual(
		"subject: a\n"
		"tester: b\n"
		"stimulus list: c\n"
		"\n"
		"spatialization\n"
		"    BRIR: d\n\n",
		writer.content()
	);
}

TEST_F(
	TestDocumenterImplTests,
	ignoresBrirIfNotUsingSpatialization
) {
	Model::Testing test;
	test.subjectId = "a";
	test.testerId = "b";
	test.audioDirectory = "c";
	test.processing.usingSpatialization = false;
	test.processing.usingHearingAidSimulation = true;
	test.processing.leftDslPrescriptionFilePath = "e";
	test.processing.rightDslPrescriptionFilePath = "f";
	test.processing.attack_ms = 1.1;
	test.processing.release_ms = 2.2;
	test.processing.windowSize = 3;
	test.processing.chunkSize = 4;
	documenter.documentTestParameters(test);
	assertEqual(
		"subject: a\n"
		"tester: b\n"
		"stimulus list: c\n"
		"\n"
		"hearing aid simulation\n"
		"    DSL prescription\n"
		"        left: e\n"
		"        right: f\n"
		"    attack (ms): 1.1\n"
		"    release (ms): 2.2\n"
		"    window size (samples): 3\n"
		"    chunk size (samples): 4\n\n", 
		writer.content()
	);
}

TEST_F(
	TestDocumenterImplTests,
	playTrialDocumentsTrial
) {
	TestDocumenter::TrialParameters trial;
	trial.stimulus = "a";
	trial.level_dB_Spl = 1;
	documenter.documentTrialParameters(trial);
	assertEqual(
		"stimulus: a\n"
		"level (dB SPL): 1\n\n", 
		writer.content()
	);
}

TEST_F(
	TestDocumenterImplTests,
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
	TestDocumenterImpl documenter{ &writer };
	try {
		documenter.initialize({});
		FAIL() << "Expected TestDocumenter::InitializationFailure";
	}
	catch (const TestDocumenter::InitializationFailure &e) {
		assertEqual(std::string{ "error." }, e.what());
	}
}