#include "assert-utility.h"
#include "FakeStimulusList.h"
#include "DocumenterStub.h"
#include <recognition-test/RecognitionTest.h>
#include <gtest/gtest.h>

namespace {
	class RecognitionTestTests : public ::testing::Test {
	protected:
		RecognitionTest::TestParameters newTest;
		FakeStimulusList list{};
		DocumenterStub documenter{};
		RecognitionTest model{ &list, &documenter };

		void prepareNewTest() {
			model.prepareNewTest(newTest);
		}

		void advanceTrial() {
			model.advanceTrial();
		}
	};

	TEST_F(
		RecognitionTestTests,
		prepareNewTestInitializesStimulusList
	) {
		newTest.stimulusList = "a";
		prepareNewTest();
		assertEqual("a", list.directory());
	}

	TEST_F(
		RecognitionTestTests,
		prepareNewTestInitializesDocumenter
	) {
		newTest.outputFilePath = "a";
		prepareNewTest();
		assertEqual("a", documenter.filePath());
	}

	TEST_F(
		RecognitionTestTests,
		prepareNewTestDocumentsTestParameters
	) {
		newTest.subjectId = "a";
		newTest.testerId = "b";
		prepareNewTest();
		assertEqual("a", documenter.documentedTestParameters().subjectId);
		assertEqual("b", documenter.documentedTestParameters().testerId);
	}

	TEST_F(
		RecognitionTestTests,
		prepareNewTestDocumentsTestParametersAfterInitializing
	) {
		prepareNewTest();
		assertTrue(documenter.log().beginsWith("initialize"));
	}

	TEST_F(
		RecognitionTestTests,
		nextStimulusReturnsThatOfList
	) {
		list.setContents({ "a", "b", "c" });
		prepareNewTest();
		assertEqual("a", model.nextStimulus());
		advanceTrial();
		assertEqual("b", model.nextStimulus());
		advanceTrial();
		assertEqual("c", model.nextStimulus());
	}

	TEST_F(
		RecognitionTestTests,
		playNextTrialDocumentsTrial
	) {
		list.setContents({ "a", "b", "c" });
		prepareNewTest();
		advanceTrial();
		assertEqual("a", documenter.documentedTrialParameters().stimulus);
	}

	TEST_F(
		RecognitionTestTests,
		testCompleteWhenListEmpty
	) {
		list.setContents({});
		assertTrue(model.testComplete());
	}

	class RecognitionTestModelWithInitializationFailingDocumenter : public ::testing::Test {
	protected:
		FakeStimulusList list{};
		InitializationFailingDocumenter documenter{};
		RecognitionTest model{ &list, &documenter };

		void assertPrepareNewTestThrowsInitializationFailure(std::string what) {
			try {
				model.prepareNewTest({});
				FAIL() << "Expected RecognitionTest::TestInitializationFailure";
			}
			catch (const RecognitionTest::TestInitializationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
		}
	};

	TEST_F(
		RecognitionTestModelWithInitializationFailingDocumenter,
		prepareNewTestThrowsInitializationFailureWhenDocumenterFailsToInitialize
	) {
		documenter.setErrorMessage("error.");
		assertPrepareNewTestThrowsInitializationFailure("error.");
	}
}