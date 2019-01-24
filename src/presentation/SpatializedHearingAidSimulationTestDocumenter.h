#pragma once

#include <common-includes/Interface.h>
#include <string>

class PersistentMemoryWriter {
public:
	INTERFACE_OPERATIONS(PersistentMemoryWriter);
	virtual void write(std::string) = 0;
};

#include <presentation/Presenter.h>

#include <sstream>
#include <iomanip>

class FormattedStream {
	std::stringstream stream{};
public:
	void insertFixed() {
		stream << std::fixed;
	}

	void insertPrecision(std::streamsize n) {
		stream << std::setprecision(n);
	}

	template<typename T>
	void insertLabeledParameterLine(std::string label, T p) {
		stream << std::move(label) << ": " << std::move(p) << '\n';
	}

	template<typename T>
	void insertIndentedLabeledParameterLine(std::string label, T p) {
		stream << "    ";
		insertLabeledParameterLine(std::move(label), std::move(p));
	}

	void insertLine(std::string s = {}) {
		stream << std::move(s) << '\n';
	}

	std::string str() {
		return stream.str();
	}
};

class SpatializedHearingAidSimulationTestDocumenter {
	PersistentMemoryWriter *writer;
public:
	explicit SpatializedHearingAidSimulationTestDocumenter(PersistentMemoryWriter *writer) :
		writer{ writer } {}

	void documentTestParameters(GlobalTestParameters *global) {
		FormattedStream stream;
		stream.insertLabeledParameterLine("subject", global->subjectId);
		stream.insertLabeledParameterLine("tester",  global->testerId);
		stream.insertLine("DSL prescription");
		stream.insertIndentedLabeledParameterLine("left",  global->leftDslPrescriptionFilePath);
		stream.insertIndentedLabeledParameterLine("right",  global->rightDslPrescriptionFilePath);
		stream.insertLabeledParameterLine("BRIR",  global->brirFilePath);
		stream.insertFixed();
		stream.insertPrecision(1);
		stream.insertLabeledParameterLine("attack (ms)",  global->attack_ms);
		stream.insertLabeledParameterLine("release (ms)",  global->release_ms);
		stream.insertLabeledParameterLine("window size (samples)",  global->windowSize);
		stream.insertLabeledParameterLine("chunk size (samples)",  global->chunkSize);
		stream.insertLine();
		writer->write(stream.str());
	}
};

