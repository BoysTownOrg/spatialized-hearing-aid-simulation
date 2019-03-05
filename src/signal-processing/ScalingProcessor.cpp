#include "ScalingProcessor.h"

template<typename T>
ScalingProcessor<T>::ScalingProcessor(T scale) : scale{ scale } {}

template<typename T>
void ScalingProcessor<T>::process(signal_type signal) {
	for (auto &x : signal)
		x *= scale;
}

template<typename T>
auto ScalingProcessor<T>::groupDelay() -> index_type {
	return index_type{ 0 };
}

template ScalingProcessor<float>;
