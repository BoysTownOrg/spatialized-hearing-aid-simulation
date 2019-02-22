#include "LibsndfileReader.h"

LibsndfileReader::LibsndfileReader(std::string filePath) {
	file = sf_open(filePath.c_str(), SFM_READ, &info);
}

LibsndfileReader::~LibsndfileReader() {
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

LibsndfileWriter::~LibsndfileWriter() {
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