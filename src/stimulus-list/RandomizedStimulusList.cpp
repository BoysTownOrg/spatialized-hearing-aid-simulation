#include "RandomizedStimulusList.h"

RandomizedStimulusList::RandomizedStimulusList(
	DirectoryReaderFactory *factory,
    Randomizer *randomizer
) :
    factory{ factory },
    randomizer{ randomizer } {}

void RandomizedStimulusList::initialize(std::string directory) {
	auto reader = factory->make(directory);
	if (reader->failed())
		throw InitializationFailure{ reader->errorMessage() };
    files = reader->files();
    randomizer->shuffle(files.begin(), files.end());
    directory_ = std::move(directory);
}

bool RandomizedStimulusList::empty() {
    return files.empty();
}

std::string RandomizedStimulusList::next() {
	if (files.size()) {
		current_ = files.front();
		files.erase(files.begin());
	}
    return directory_ + "/" + current_;
}
