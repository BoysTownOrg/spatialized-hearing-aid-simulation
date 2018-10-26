#include <audio-file-reading/AudioFileInMemory.h>
#include <sndfile.h>

class LibsndfileReader : public AudioFileReader {
	SNDFILE *file{};
	SF_INFO info{};
public:
	explicit LibsndfileReader(std::string filePath);
	~LibsndfileReader();
	void readFrames(float *, long long) override;
	long long frames() override;
	int channels() override;
};

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

int main() {}