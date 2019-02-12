#include "FileFilterDecorator.h"

FileFilterDecorator::FileFilterDecorator(
	std::shared_ptr<DirectoryReader> reader,
    std::string filter
) :
    filter{ std::move(filter) },
    reader{ std::move(reader) } {}

std::vector<std::string> FileFilterDecorator::files() {
    return filtered(reader->files());
}

std::vector<std::string> FileFilterDecorator::filtered(std::vector<std::string> files) {
    std::vector<std::string> filtered_{};
    for (auto &file : files)
        if (file.length() >= filter.length())
            if (0 == file.compare(file.length() - filter.length(), filter.length(), filter))
                filtered_.push_back(file);
    return filtered_;
}

bool FileFilterDecorator::failed() {
	return reader->failed();
}

std::string FileFilterDecorator::errorMessage() {
	return reader->errorMessage();
}

FileFilterDecoratorFactory::FileFilterDecoratorFactory(
	DirectoryReaderFactory * decorated, 
	std::string filter
) :
	decorated{ decorated },
	filter{ std::move(filter) }
{
}

std::shared_ptr<DirectoryReader> FileFilterDecoratorFactory::make(std::string directory)
{
	return std::make_shared<FileFilterDecorator>(decorated->make(std::move(directory)), filter);
}
