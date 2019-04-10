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
		RandomizedStimulusList stimulusList{ &factory, &randomizer };

		auto initialize(std::string d = {}) {
			return stimulusList.initialize(std::move(d));
		}

		auto next() {
			return stimulusList.next();
		}

		void assertInitializeThrowsInitializationFailure(std::string what) {
			try {
				stimulusList.initialize({});
				FAIL() << "Expected RandomizedStimulusList::InitializationFailure";
			}
			catch (const RandomizedStimulusList::InitializationFailure &e) {
				assertEqual(std::move(what), e.what());
			}
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
		assertFalse(stimulusList.empty());
		next();
		assertFalse(stimulusList.empty());
		next();
		assertTrue(stimulusList.empty());
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
		assertEqual<std::string>({ "a", "b", "c" }, randomizer.toShuffle());
	}

	TEST_F(
		RandomizedStimulusListTests,
		initializeThrowsInitializationFailureWhenDirectoryReaderFails
	) {
		reader->fail();
		reader->setErrorMessage("error.");
		assertInitializeThrowsInitializationFailure("error.");
	}

	class RandomizedStimulusListFailureTests : public ::testing::Test {
	protected:
		std::shared_ptr<DirectoryReaderStub> reader 
			= std::make_shared<DirectoryReaderStub>();
		DirectoryReaderStubFactory factory{reader};
		RandomizerStub randomizer{};
	};

	class FileFilterDecoratorTests : public ::testing::Test {
	protected:
		std::shared_ptr<DirectoryReaderStub> reader 
			= std::make_shared<DirectoryReaderStub>();
		DirectoryReaderStubFactory factory{reader};
		std::string filter{};

		std::shared_ptr<DirectoryReader> makeDecorator(std::string f = {}) {
			FileFilterDecoratorFactory decorator{ &factory, filter };
			return decorator.make(f);
		}
	};

	TEST_F(FileFilterDecoratorTests, passesDirectoryToDecorated) {
		auto decorator = makeDecorator("a");
		assertEqual("a", factory.directory());
	}

	TEST_F(FileFilterDecoratorTests, returnsFilteredFiles) {
		filter = ".c";
		auto decorator = makeDecorator();
		reader->setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
		assertEqual({ "b.c", "f.c" }, decorator->files());
	}

	TEST_F(FileFilterDecoratorTests, returnsDecoratedQueries) {
		auto decorator = makeDecorator();
		assertFalse(decorator->failed());
		reader->fail();
		reader->setErrorMessage("a");
		assertTrue(decorator->failed());
		assertEqual("a", decorator->errorMessage());
	}
}
