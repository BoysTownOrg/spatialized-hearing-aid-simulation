#include "assert-utility.h"
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>
#include "DirectoryReaderStub.h"
#include "RandomizerStub.h"
#include <gtest/gtest.h>

namespace {
	class RandomizedStimulusListTests : public ::testing::Test {
	protected:
		std::shared_ptr<DirectoryReaderStub> reader 
			= std::make_shared<DirectoryReaderStub>();
		DirectoryReaderStubFactory factory{reader};
		RandomizerStub randomizer{};
		RandomizedStimulusList list{ &factory, &randomizer };

		auto initialize(std::string d = {}) {
			return list.initialize(std::move(d));
		}

		auto next() {
			return list.next();
		}
	};

	TEST_F(
		RandomizedStimulusListTests,
		initializePassesDirectoryToFactory
	) {
		initialize("a");
		assertEqual("a", factory.directory());
	}

	TEST_F(
		RandomizedStimulusListTests,
		emptyWhenExhausted
	) {
		reader->setFileNames({ "a", "b" });
		initialize();
		assertFalse(list.empty());
		next();
		assertFalse(list.empty());
		next();
		assertTrue(list.empty());
	}

	TEST_F(
		RandomizedStimulusListTests,
		nextReturnsFullPathToFileAtFront
	) {
		reader->setFileNames({ "a", "b", "c" });
		initialize({ "C:" });
		assertEqual("C:/a", next());
		assertEqual("C:/b", next());
		assertEqual("C:/c", next());
	}

	TEST_F(
		RandomizedStimulusListTests,
		nextReturnsFullPathToLastFileWhenExhausted
	) {
		reader->setFileNames({ "a" });
		initialize({ "C:" });
		assertEqual("C:/a", next());
		assertEqual("C:/a", next());
	}

	TEST_F(
		RandomizedStimulusListTests,
		initializeShufflesFileNames
	) {
		reader->setFileNames({ "a", "b", "c" });
		initialize();
		assertEqual({ "a", "b", "c" }, randomizer.toShuffle());
	}

	class FileFilterDecoratorTests : public ::testing::Test {
	protected:
		DirectoryReaderStub reader{};

		FileFilterDecorator makeDecorator(std::string f = {}) {
			return FileFilterDecorator{ &reader, f };
		}
	};

	TEST_F(FileFilterDecoratorTests, returnsFilteredFiles) {
		auto decorator = makeDecorator(".c");
		reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
		assertEqual({ "b.c", "f.c" }, decorator.files());
	}
}
