#pragma once

#include <hearing-aid-processing/FilterbankCompressor.h>
extern "C" {
#include <chapro.h>
}

// These are (unfortunately) defined in chapro.h but appear in some standard headers
#undef _size
#undef fmin
#undef fmove
#undef fcopy
#undef fzero
#undef dcopy
#undef dzero
#undef round
#undef log2

class Chapro : public FilterbankCompressor {
	void *cha_pointer[NPTR]{};
	const int channels_;
	const int chunkSize_;
	const int windowSize_;
	int error = 0;
public:
	explicit Chapro(Parameters);
	~Chapro() noexcept;
	Chapro(Chapro &&) = delete;
	Chapro &operator=(Chapro &&) = delete;
	Chapro(const Chapro &) = delete;
	Chapro &operator=(const Chapro &) = delete;
	void compressInput(real_type * input, real_type * output, int chunkSize) override;
	void analyzeFilterbank(real_type * input, complex_type * output, int chunkSize) override;
	void compressChannels(complex_type * input, complex_type * output, int chunkSize) override;
	void synthesizeFilterbank(complex_type * input, real_type * output, int chunkSize) override;
	void compressOutput(real_type * input, real_type * output, int chunkSize) override;
	int chunkSize() const override;
	int channels() const override;
	bool failed() const override;
	int windowSize() const override;
};

class ChaproFactory : public FilterbankCompressorFactory {
	std::shared_ptr<FilterbankCompressor> make(FilterbankCompressor::Parameters parameters) override {
		return std::make_shared<Chapro>(std::move(parameters));
	}
};