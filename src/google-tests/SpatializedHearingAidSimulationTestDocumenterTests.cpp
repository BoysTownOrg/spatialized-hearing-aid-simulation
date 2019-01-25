#include "assert-utility.h"
#include "PersistentMemoryWriterStub.h"
#include <spatialized-hearing-aid-simulation-utility/SpatializedHearingAidSimulationTestDocumenter.h>
#include <gtest/gtest.h>

class SpatializedHearingAidSimulationTestDocumenterTests : public ::testing::Test {
protected:
	PersistentMemoryWriterStub writer;
	SpatializedHearingAidSimulationTestDocumenter documenter{ &writer };
};

TEST_F(
	SpatializedHearingAidSimulationTestDocumenterTests,
	formatsTestParameters
) {
	GlobalTestParameters test;
	test.subjectId = "a";
	test.testerId = "b";
	test.leftDslPrescriptionFilePath = "c";
	test.rightDslPrescriptionFilePath = "d";
	test.brirFilePath = "e";
	test.attack_ms = 1;
	test.release_ms = 2;
	test.windowSize = 3;
	test.chunkSize = 4;
	SpatializedHearingAidSimulationTestDocumenter::TestParameters p;
	p.global = &test;
	documenter.documentTestParameters(p);
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
	SpatializedHearingAidSimulationTestDocumenterTests,
	ignoresPrescriptionsIfNotUsingHearingAidSimulation
) {
	GlobalTestParameters test;
	test.subjectId = "a";
	test.testerId = "b";
	test.leftDslPrescriptionFilePath = "c";
	test.rightDslPrescriptionFilePath = "d";
	test.brirFilePath = "e";
	test.attack_ms = 1;
	test.release_ms = 2;
	test.windowSize = 3;
	test.chunkSize = 4;
	test.usingHearingAidSimulation = false;
	SpatializedHearingAidSimulationTestDocumenter::TestParameters p;
	p.global = &test;
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
	SpatializedHearingAidSimulationTestDocumenterTests,
	playTrialDocumentsTrial
) {
	GlobalTrialParameters trial;
	trial.level_dB_Spl = 1;
	trial.stimulus = "a";
	SpatializedHearingAidSimulationTestDocumenter::TrialParameters p;
	p.global = &trial;
	documenter.documentTrialParameters(p);
	assertEqual(
		"stimulus: a\n"
		"level (dB SPL): 1\n\n", 
		writer.content()
	);
}

TEST_F(
	SpatializedHearingAidSimulationTestDocumenterTests,
	initializePassesFilePath
) {
	documenter.initialize("a");
	assertEqual("a", writer.filePath());
}

TEST(
	SpatializedHearingAidSimulationTestDocumenterWithInitializationFailingWriter,
	initializeTestThrowsInitializationFailureWhenDocumenterFailsToInitialize
) {
	InitializationFailingWriter writer{};
	writer.setErrorMessage("error.");
	SpatializedHearingAidSimulationTestDocumenter documenter{ &writer };
	try {
		documenter.initialize({});
		FAIL() << "Expected SpatializedHearingAidSimulationTestDocumenter::InitializationFailure";
	}
	catch (const SpatializedHearingAidSimulationTestDocumenter::InitializationFailure &e) {
		assertEqual("error.", e.what());
	}
}