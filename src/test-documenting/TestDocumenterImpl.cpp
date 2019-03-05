#include "TestDocumenterImpl.h"
#include <sstream>
#include <iomanip>

class FormattedStream {
	std::stringstream stream{};
	int indents{};
public:
	void indent() noexcept {
		++indents;
	}

	void deindent() noexcept {
		--indents;
	}

	void insertFixed() {
		stream << std::fixed;
	}

	void insertPrecision(std::streamsize n) {
		stream << std::setprecision(n);
	}

	template<typename T>
	void insertLabeledParameterLine(std::string label, T p) {
		std::stringstream line{};
		line << std::move(label) << ": " << std::move(p);
		insertLine(line.str());
	}

	void insertLine(std::string s = {}) {
		for (int i = 0; i < indents; ++i)
			stream << "    ";
		stream << std::move(s) << '\n';
	}

	std::string str() {
		return stream.str();
	}
};

TestDocumenterImpl::TestDocumenterImpl(
	PersistentMemoryWriter *writer
) :
	writer{ writer } {}

void TestDocumenterImpl::initialize(std::string filePath) {
	writer->initialize(std::move(filePath));
	if (writer->failed())
		throw InitializationFailure{ writer->errorMessage() };
}

void TestDocumenterImpl::documentTestParameters(Model::Testing *p) {
	FormattedStream stream;
	stream.insertLabeledParameterLine("subject", p->subjectId);
	stream.insertLabeledParameterLine("tester", p->testerId);
	stream.insertLabeledParameterLine("stimulus list", p->audioDirectory);
	if (p->processing.usingSpatialization) {
		stream.insertLine();
		stream.insertLine("spatialization");
		stream.indent();
		stream.insertLabeledParameterLine("BRIR", p->processing.brirFilePath);
		stream.deindent();
	}
	if (p->processing.usingHearingAidSimulation) {
		stream.insertLine();
		stream.insertLine("hearing aid simulation");
		stream.indent();
		stream.insertLine("DSL prescription");
		stream.indent();
		stream.insertLabeledParameterLine("left", p->processing.leftDslPrescriptionFilePath);
		stream.insertLabeledParameterLine("right", p->processing.rightDslPrescriptionFilePath);
		stream.deindent();
		stream.insertFixed();
		stream.insertPrecision(1);
		stream.insertLabeledParameterLine("attack (ms)", p->processing.attack_ms);
		stream.insertLabeledParameterLine("release (ms)", p->processing.release_ms);
		stream.insertLabeledParameterLine("window size (samples)", p->processing.windowSize);
		stream.insertLabeledParameterLine("chunk size (samples)", p->processing.chunkSize);
		stream.deindent();
	}
	stream.insertLine();
	writer->write(stream.str());
}

void TestDocumenterImpl::documentTrialParameters(TrialParameters p) {
	FormattedStream stream;
	stream.insertLabeledParameterLine("stimulus", p.stimulus);
	stream.insertLabeledParameterLine("level (dB SPL)", p.level_dB_Spl);
	stream.insertLine();
	writer->write(stream.str());
}
