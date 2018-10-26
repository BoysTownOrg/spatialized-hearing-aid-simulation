#pragma once
#include <vector>

template<typename T>
class FirFilter {
	using vector = std::vector<T>;
	const vector b;
	vector delayLine;
public:
	class InvalidCoefficients {};
	explicit FirFilter(vector b);
	void process(T *, int);

private:
	void filter(T *, int);
	void updateDelayLine(T *x, int n);
};

#include <algorithm>

template<typename T>
FirFilter<T>::FirFilter(vector b) :
	b(std::move(b))
{
	if (this->b.size() > 1)
		delayLine.resize(this->b.size() - 1, 0);
	else if (this->b.size() == 0)
		throw InvalidCoefficients{};
}

template<typename T>
void FirFilter<T>::process(T *x, int n)
{
	filter(x, n);
	updateDelayLine(x, n);
}

template<typename T>
void FirFilter<T>::filter(T *x, int n)
{
	const auto size = static_cast<std::size_t>(n);
	for (std::size_t i = size; i > 0; --i) {
		T accumulate = 0;
		for (
			std::size_t coefficientIndex = 0;
			coefficientIndex < b.size();
			++coefficientIndex)
		{
			const auto delayedInput = i > coefficientIndex
				? x[i - 1 - coefficientIndex]
				: *(delayLine.end() - coefficientIndex + i - 1);
			accumulate += b[coefficientIndex] * delayedInput;
		}
		x[i - 1] = accumulate;
	}
}

template<typename T>
void FirFilter<T>::updateDelayLine(T *x, int n)
{
	const auto size = static_cast<std::size_t>(n);
	for (std::size_t i = 0; i + size < delayLine.size(); ++i)
		delayLine[i] = delayLine[i + size];
	for (std::size_t i = 0; i < std::min(delayLine.size(), size); ++i)
		*(delayLine.end() - i - 1) = *(x - size - i - 1);
}

template class FirFilter<float>;
template class FirFilter<double>;

