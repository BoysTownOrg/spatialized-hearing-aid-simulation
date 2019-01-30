#include "TestDocumenter.h"
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

TestDocumenter::TestDocumenter(
	PersistentMemoryWriter *writer
) :
	writer{ writer } {}

void TestDocumenter::initialize(std::string filePath) {
	writer->initialize(std::move(filePath));
	if (writer->failed())
		throw InitializationFailure{ writer->errorMessage() };
}

void TestDocumenter::documentTestParameters(TestParameters) {
	FormattedStream stream;
	stream.insertFixed();
	stream.insertPrecision(1);
	stream.insertLine();
	writer->write(stream.str());
}

void TestDocumenter::documentTrialParameters(TrialParameters p) {
	FormattedStream stream;
	stream.insertLabeledParameterLine("stimulus", p.stimulus);
	//stream.insertLabeledParameterLine("level (dB SPL)", global->level_dB_Spl);
	stream.insertLine();
	writer->write(stream.str());
}
