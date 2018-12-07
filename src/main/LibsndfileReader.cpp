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

void LibsndfileReader::readFrames(double *x, long long count) {
	sf_readf_double(file, x, count);
}

long long LibsndfileReader::frames() {
	return info.frames;
}

int LibsndfileReader::channels() {
	return info.channels;
}

bool LibsndfileReader::failed() const {
	return file == nullptr;
}

std::string LibsndfileReader::errorMessage() const {
	return sf_strerror(file);
}

int LibsndfileReader::sampleRate() {
	return info.samplerate;
}
