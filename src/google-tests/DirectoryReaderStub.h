#pragma once

#include <stimulus-list/DirectoryReader.h>

class DirectoryReaderStub : public DirectoryReader {
    std::vector<std::string> fileNames_{};
    std::string directory_{};
public:
    void setFileNames(std::vector<std::string> files) noexcept {
        fileNames_ = std::move(files);
    }
    
    std::vector<std::string> filesIn(std::string directory) override {
        directory_ = std::move(directory);
        return fileNames_;
    }
    
    auto directory() const {
        return directory_;
    }
};

class DirectoryReaderStubFactory : public DirectoryReaderFactory {
	std::string directory_{};
	std::shared_ptr<DirectoryReader> reader;
public:
	explicit DirectoryReaderStubFactory(std::shared_ptr<DirectoryReader> reader) : 
		reader{ std::move(reader) } {}

	std::shared_ptr<DirectoryReader> make(std::string d) {
		directory_ = std::move(d);
		return reader;
	}

	auto directory() const {
		return directory_;
	}
};