#include "assert-utility.h"
#include "DocumenterStub.h"
#include <spatialized-hearing-aid-simulation-utility/SpatializedHearingAidSimulationTestDocumenter.h>
#include <gtest/gtest.h>
#include <sstream>

TEST(
	SpatializedHearingAidSimulationTestDocumenterTests,
	formatsTestParameters
) {
	GlobalTestParameters x;
	x.subjectId = "a";
	x.testerId = "b";
	x.leftDslPrescriptionFilePath = "c";
	x.rightDslPrescriptionFilePath = "d";
	x.brirFilePath = "e";
	x.attack_ms = 1;
	x.release_ms = 2;
	x.windowSize = 3;
	x.chunkSize = 4;
	PersistentMemoryWriterStub writer;
	SpatializedHearingAidSimulationTestDocumenter documenter{ &writer };
	documenter.documentTestParameters(&x);
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

TEST(
	SpatializedHearingAidSimulationTestDocumenterTests,
	playTrialDocumentsTrial
) {
	GlobalTrialParameters x;
	x.level_dB_Spl = 1;
	x.stimulus = "a";
	PersistentMemoryWriterStub writer;
	SpatializedHearingAidSimulationTestDocumenter documenter{ &writer };
	documenter.documentTrialParameters(&x);
	assertEqual(
		"stimulus: a\n"
		"level (dB SPL): 1\n\n", 
		writer.content()
	);
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