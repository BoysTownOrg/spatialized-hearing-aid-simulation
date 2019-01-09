#include <recognition-test/RecognitionTestModel.h>
#include <common-includes/Interface.h>

class DirectoryReader {
public:
    INTERFACE_OPERATIONS(DirectoryReader);
    virtual std::vector<std::string> filesIn(std::string directory) = 0;
};
    
class Randomizer {
public:
    INTERFACE_OPERATIONS(Randomizer);
    using shuffle_iterator = std::vector<std::string>::iterator;
    virtual void shuffle(shuffle_iterator begin, shuffle_iterator end) = 0;
};

class RandomizedStimulusList : public StimulusList {
    std::vector<std::string> files{};
    std::string directory_{};
    DirectoryReader *reader;
    Randomizer *randomizer;
public:
    RandomizedStimulusList(DirectoryReader *reader, Randomizer *randomizer);
    void initialize(std::string directory) override;
    bool empty() override;
    std::string next() override;
};

RandomizedStimulusList::RandomizedStimulusList(
    DirectoryReader *reader,
    Randomizer *randomizer
) :
    reader{reader},
    randomizer{randomizer} {}
    
void RandomizedStimulusList::initialize(std::string directory) {
    directory_ = std::move(directory);
    files = reader->filesIn(directory_);
    randomizer->shuffle(files.begin(), files.end());
}
    
bool RandomizedStimulusList::empty() {
    return files.empty();
}
    
std::string RandomizedStimulusList::next() {
    auto next_ = files.front();
    files.erase(files.begin());
    return directory_ + "/" + next_;
}

class FileFilterDecorator : public DirectoryReader {
    std::string filter;
    DirectoryReader *reader;
public:
    FileFilterDecorator(DirectoryReader *reader, std::string filter);
    std::vector<std::string> filesIn(std::string directory) override;
private:
    std::vector<std::string> filtered(std::vector<std::string>);
};

FileFilterDecorator::FileFilterDecorator(
    DirectoryReader *reader,
    std::string filter
) :
    filter{filter},
    reader{reader} {}

std::vector<std::string> FileFilterDecorator::filesIn(std::string directory) {
    return filtered(reader->filesIn(directory));
}
    
std::vector<std::string> FileFilterDecorator::filtered(std::vector<std::string> files) {
    std::vector<std::string> filtered_{};
    for (auto &file : files)
        if (file.length() >= filter.length())
            if (0 == file.compare(file.length() - filter.length(), filter.length(), filter))
                filtered_.push_back(file);
    return filtered_;
}

#include "assert-utility.h"
#include <gtest/gtest.h>

class DirectoryReaderStub : public DirectoryReader {
    std::vector<std::string> fileNames_{};
    std::string directory_{};
public:
    void setFileNames(std::vector<std::string> files) {
        fileNames_ = files;
    }
    
    std::vector<std::string> filesIn(std::string directory) override {
        directory_ = directory;
        return fileNames_;
    }
    
    std::string directory() const {
        return directory_;
    }
};

class RandomizerStub : public Randomizer {
    std::vector<std::string> toShuffle_;
public:
    std::vector<std::string> toShuffle() const {
        return toShuffle_;
    }
    
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        toShuffle_ = {begin, end};
    }
};

class RandomizedStimulusListTests : public ::testing::Test {
protected:
    DirectoryReaderStub reader{};
    RandomizerStub randomizer{};
    RandomizedStimulusList list{&reader, &randomizer};
};

TEST_F(
    RandomizedStimulusListTests,
    initializePassesDirectoryToDirectoryReader
) {
    list.initialize("a");
    assertEqual("a", reader.directory());
}

TEST_F(
    RandomizedStimulusListTests,
    emptyWhenEmpty
) {
    reader.setFileNames({ "a", "b", "c" });
    list.initialize({});
    EXPECT_FALSE(list.empty());
    list.next();
    EXPECT_FALSE(list.empty());
    list.next();
    EXPECT_FALSE(list.empty());
    list.next();
    EXPECT_TRUE(list.empty());
}

TEST_F(
    RandomizedStimulusListTests,
    nextReturnsFullPathToFileAtFront
) {
    reader.setFileNames({ "a", "b", "c" });
    list.initialize({"C:"});
    assertEqual("C:/a", list.next());
    assertEqual("C:/b", list.next());
    assertEqual("C:/c", list.next());
}

TEST_F(
    RandomizedStimulusListTests,
    initializeShufflesFileNames
) {
    reader.setFileNames({ "a", "b", "c" });
    list.initialize({});
    assertEqual({ "a", "b", "c" }, randomizer.toShuffle());
}

TEST(FileFilterDecoratorTests, passesDirectoryToDecorated) {
    DirectoryReaderStub reader;
    FileFilterDecorator decorator{&reader, {}};
    decorator.filesIn({"a"});
    assertEqual("a", reader.directory());
}

TEST(FileFilterDecoratorTests, returnsFilteredFiles) {
    DirectoryReaderStub reader;
    FileFilterDecorator decorator{&reader, ".c"};
    reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
    assertEqual({ "b.c", "f.c" }, decorator.filesIn({}));
}

