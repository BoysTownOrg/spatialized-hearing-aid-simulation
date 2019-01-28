#pragma once

#include <test-documenting/PersistentMemoryWriter.h>
#include <sstream>

class PersistentMemoryWriterStub : public PersistentMemoryWriter {
	std::stringstream content_{};
	std::string filePath_{};
public:
	void write(std::string s) override {
		content_ << std::move(s);
	}

	std::string content() const {
		return content_.str();
	}

	std::string filePath() const {
		return filePath_;
	}

	void initialize(std::string s) override {
		filePath_ = std::move(s);
	}

	bool failed() override { return {}; }
	std::string errorMessage() override { return {}; }
};

class InitializationFailingWriter : public PersistentMemoryWriter {
	std::string errorMessage_{};
	bool failed_{};
public:
	void setErrorMessage(std::string s) {
		errorMessage_ = std::move(s);
	}

	void initialize(std::string) override {
		failed_ = true;
	}

	bool failed() override {
		return failed_;
	}

	std::string errorMessage() override {
		return errorMessage_;
	}

	void write(std::string) override {}
};