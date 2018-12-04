#pragma once

#include <dsl-prescription/DslPrescription.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
extern "C" {
#include <chapro.h>
}
#undef _size

class Chapro : public FilterbankCompressor {
	void *cha_pointer[NPTR]{};
	const int _channels;
	const int _chunkSize;
	int error = 0;
public:
	Chapro(
		const DslPrescription &,
		Parameters);
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
};

class ChaproFactory : public FilterbankCompressorFactory {
	std::shared_ptr<FilterbankCompressor> make(
		const DslPrescription &prescription,
		FilterbankCompressor::Parameters parameters) override 
	{
		return std::make_shared<Chapro>(prescription, parameters);
	}
};