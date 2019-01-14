#include "FileFilterDecorator.h"

FileFilterDecorator::FileFilterDecorator(
    DirectoryReader *reader,
    std::string filter
) :
    filter{ std::move(filter) },
    reader{ reader } {}

std::vector<std::string> FileFilterDecorator::filesIn(std::string directory) {
    return filtered(reader->filesIn(std::move(directory)));
}

std::vector<std::string> FileFilterDecorator::filtered(std::vector<std::string> files) {
    std::vector<std::string> filtered_{};
    for (auto &file : files)
        if (file.length() >= filter.length())
            if (0 == file.compare(file.length() - filter.length(), filter.length(), filter))
                filtered_.push_back(file);
    return filtered_;
}
