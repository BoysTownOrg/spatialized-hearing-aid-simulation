#include "assert-utility.h"
#include "StimulusPlayerStub.h"
#include "FakeStimulusList.h"
#include "DocumenterStub.h"
#include <recognition-test/RecognitionTest.h>
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
	class RecognitionTestTests : public ::testing::Test {
	protected:
		RecognitionTest::TestParameters newTest;
		FakeStimulusList list{};
		StimulusPlayerStub player{};
		DocumenterStub documenter{};
		RecognitionTest model{ &list, &documenter };

		void prepareNewTest() {
			model.prepareNewTest(newTest);
		}

		void playNextTrial() {
			model.playNextTrial(&player);
		}
	};

	TEST_F(
		RecognitionTestTests,
		prepareNewTestInitializesStimulusList
	) {
		newTest.audioDirectory = "a";
		prepareNewTest();
		assertEqual("a", list.directory());
	}

	TEST_F(
		RecognitionTestTests,
		prepareNewTestInitializesDocumenter
	) {
		newTest.testFilePath = "a";
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
		EXPECT_TRUE(documenter.log().beginsWith("initialize"));
	}

	TEST_F(
		RecognitionTestTests,
		nextStimulusReturnsThatOfList
	) {
		list.setContents({ "a", "b", "c" });
		prepareNewTest();
		assertEqual("a", model.nextStimulus());
		playNextTrial();
		assertEqual("b", model.nextStimulus());
		playNextTrial();
		assertEqual("c", model.nextStimulus());
	}

	TEST_F(RecognitionTestTests, playNextTrialDoesNotAdvanceListWhenPlayerPlaying) {
		list.setContents({ "a", "b", "c" });
		prepareNewTest();
		player.setPlaying();
		playNextTrial();
		assertEqual("a", model.nextStimulus());
	}

	TEST_F(
		RecognitionTestTests,
		playNextTrialDocumentsTrial
	) {
		list.setContents({ "a", "b", "c" });
		prepareNewTest();
		playNextTrial();
		assertEqual("a", documenter.documentedTrialParameters().stimulus);
	}

	TEST_F(
		RecognitionTestTests,
		playNextTrialPlaysPlayer
	) {
		playNextTrial();
		EXPECT_TRUE(player.played());
	}

	TEST_F(
		RecognitionTestTests,
		testCompleteWhenListEmpty
	) {
		list.setContents({});
		EXPECT_TRUE(model.testComplete());
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