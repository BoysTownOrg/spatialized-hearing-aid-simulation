#pragma once

#include <dsl-prescription/DslPrescription.h>
#include <common-includes/Interface.h>
#include <memory>

class FilterbankCompressor {
public:
	struct Parameters {
		double attack_ms;
		double release_ms;
		double sampleRate;
		double max_dB;
		int windowSize;
		int chunkSize;
	};
	INTERFACE_OPERATIONS(FilterbankCompressor);
	using real_type = float;
	using complex_type = float;
	virtual void compressInput(
		real_type *input,
		real_type *output,
		int chunkSize) = 0;
	virtual void analyzeFilterbank(
		real_type *input,
		complex_type *output,
		int chunkSize) = 0;
	virtual void compressChannels(
		complex_type *input,
		complex_type *output,
		int chunkSize) = 0;
	virtual void synthesizeFilterbank(
		complex_type *input,
		real_type *output,
		int chunkSize) = 0;
	virtual void compressOutput(
		real_type *input,
		real_type *output,
		int chunkSize) = 0;
	virtual int chunkSize() const = 0;
	virtual int channels() const = 0;
	virtual bool failed() const = 0;
};

class FilterbankCompressorFactory {
public:
	INTERFACE_OPERATIONS(FilterbankCompressorFactory);
	virtual std::shared_ptr<FilterbankCompressor> make(
		const DslPrescription &,
		FilterbankCompressor::Parameters) = 0;
};
