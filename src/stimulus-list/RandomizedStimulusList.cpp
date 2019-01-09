#include "RandomizedStimulusList.h"

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
