#include "assert-utility.h"
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>
#include "DirectoryReaderStub.h"
#include "RandomizerStub.h"
#include <gtest/gtest.h>

namespace {
	class RandomizedStimulusListTests : public ::testing::Test {
	protected:
		DirectoryReaderStub reader{};
		RandomizerStub randomizer{};
		RandomizedStimulusList list{ &reader, &randomizer };
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
		assertFalse(list.empty());
		list.next();
		assertFalse(list.empty());
		list.next();
		assertFalse(list.empty());
		list.next();
		assertTrue(list.empty());
	}

	TEST_F(
		RandomizedStimulusListTests,
		nextReturnsFullPathToFileAtFront
	) {
		reader.setFileNames({ "a", "b", "c" });
		list.initialize({ "C:" });
		assertEqual("C:/a", list.next());
		assertEqual("C:/b", list.next());
		assertEqual("C:/c", list.next());
	}

	TEST_F(
		RandomizedStimulusListTests,
		nextReturnsFullPathToLastFileWhenExhausted
	) {
		reader.setFileNames({ "a", "b", "c" });
		list.initialize({ "C:" });
		list.next();
		list.next();
		list.next();
		assertEqual("C:/c", list.next());
		assertEqual("C:/c", list.next());
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
		FileFilterDecorator decorator{ &reader, {} };
		decorator.filesIn({ "a" });
		assertEqual("a", reader.directory());
	}

	TEST(FileFilterDecoratorTests, returnsFilteredFiles) {
		DirectoryReaderStub reader;
		FileFilterDecorator decorator{ &reader, ".c" };
		reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
		assertEqual({ "b.c", "f.c" }, decorator.filesIn({}));
	}
}
