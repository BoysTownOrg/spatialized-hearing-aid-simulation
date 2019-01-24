
#include <presentation/Presenter.h>
#include <common-includes/Interface.h>

class PersistentMemoryWriter {
public:
	INTERFACE_OPERATIONS(PersistentMemoryWriter);
};

class SpatializedHearingAidSimulationTestDocumenter {
public:
	explicit SpatializedHearingAidSimulationTestDocumenter(PersistentMemoryWriter *) {}
	void documentTestParameters(GlobalTestParameters *) {}
};

#include "assert-utility.h"
#include <gtest/gtest.h>
#include <sstream>

class PersistentMemoryWriterStub : public PersistentMemoryWriter {
	std::stringstream content_{};
public:
	void write(std::string s) {
		content_ << std::move(s);
	}

	std::string content() const {
		return content_.str();
	}
};

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