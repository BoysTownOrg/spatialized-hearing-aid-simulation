#include "Libsndfile.h"

LibsndfileReader::LibsndfileReader(std::string filePath) {
	file = sf_open(filePath.c_str(), SFM_READ, &info);
}

LibsndfileReader::~LibsndfileReader() noexcept {
	sf_close(file);
}

void LibsndfileReader::readFrames(float *x, long long count) {
	sf_readf_float(file, x, count);
}

long long LibsndfileReader::frames() {
	return info.frames;
}

int LibsndfileReader::channels() {
	return info.channels;
}

bool LibsndfileReader::failed() {
	return file == nullptr;
}

std::string LibsndfileReader::errorMessage() {
	return sf_strerror(file);
}

int LibsndfileReader::sampleRate() {
	return info.samplerate;
}

LibsndfileWriter::LibsndfileWriter(std::string filePath, const AudioFileWriter::AudioFileFormat &format) {
    SF_INFO info{};
    info.channels = format.channels;
    info.samplerate = format.sampleRate;
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	file = sf_open(filePath.c_str(), SFM_WRITE, &info);
}

LibsndfileWriter::~LibsndfileWriter() noexcept {
    sf_close(file);
}

bool LibsndfileWriter::failed() {
    return file == nullptr;
}

std::string LibsndfileWriter::errorMessage() {
    return sf_strerror(nullptr);
}

void LibsndfileWriter::writeFrames(float *y, long long count) {
    sf_writef_float(file, y, count);
}

std::shared_ptr<AudioFileReader> LibsndfileFactory::makeReader(std::string filePath) {
	return std::make_shared<LibsndfileReader>(std::move(filePath));
}

std::shared_ptr<AudioFileWriter> LibsndfileFactory::makeWriter(
    std::string filePath,
    const AudioFileWriter::AudioFileFormat &format
) {
	return std::make_shared<LibsndfileWriter>(std::move(filePath), format);
}
