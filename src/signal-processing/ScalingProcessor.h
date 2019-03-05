#pragma once

#include <gsl/gsl>

#ifdef SIGNAL_PROCESSING_EXPORTS
	#define SIGNAL_PROCESSING_API __declspec(dllexport)
#else
	#define SIGNAL_PROCESSING_API __declspec(dllimport)
#endif

template<typename T>
class ScalingProcessor {
	T scale;
public:
	using signal_type = gsl::span<T>;
	using index_type = typename signal_type::index_type;
	SIGNAL_PROCESSING_API explicit ScalingProcessor(T scale);
	SIGNAL_PROCESSING_API void process(signal_type signal);
	SIGNAL_PROCESSING_API index_type groupDelay();
};

