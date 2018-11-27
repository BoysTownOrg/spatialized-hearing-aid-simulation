#pragma once

#include <common-includes/Interface.h>

class FilterbankCompressor {
public:
	struct Parameters {
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
		int sampleRate;
	};
	INTERFACE_OPERATIONS(FilterbankCompressor);
	using real = float;
	using complex = float;
	virtual void compressInput(
		real *input,
		real *output,
		int chunkSize) = 0;
	virtual void analyzeFilterbank(
		real *input,
		complex *output,
		int chunkSize) = 0;
	virtual void compressChannels(
		complex *input,
		complex *output,
		int chunkSize) = 0;
	virtual void synthesizeFilterbank(
		complex *input,
		real *output,
		int chunkSize) = 0;
	virtual void compressOutput(
		real *input,
		real *output,
		int chunkSize) = 0;
	virtual int chunkSize() const = 0;
	virtual int channels() const = 0;
	virtual bool failed() const = 0;
};

#include <dsl-prescription/DslPrescription.h>
#include <memory>

class FilterbankCompressorFactory {
public:
	INTERFACE_OPERATIONS(FilterbankCompressorFactory);
	virtual std::shared_ptr<FilterbankCompressor> make(
		const DslPrescription &,
		FilterbankCompressor::Parameters) = 0;
};
