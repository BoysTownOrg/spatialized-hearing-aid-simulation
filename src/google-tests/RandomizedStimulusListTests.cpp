#include "assert-utility.h"
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>
#include <gtest/gtest.h>

class DirectoryReaderStub : public DirectoryReader {
    std::vector<std::string> fileNames_{};
    std::string directory_{};
public:
    void setFileNames(std::vector<std::string> files) {
        fileNames_ = std::move(files);
    }
    
    std::vector<std::string> filesIn(std::string directory) override {
        directory_ = std::move(directory);
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
    nextReturnsFullPathToPreviousFileWhenExhausted
) {
    reader.setFileNames({ "a", "b", "c" });
    list.initialize({"C:"});
	list.next();
	list.next();
	list.next();
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

