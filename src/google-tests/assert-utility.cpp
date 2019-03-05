#include "assert-utility.h"
#include <gtest/gtest.h>

void assertTrue(bool condition) noexcept {
	EXPECT_TRUE(condition);
}

void assertFalse(bool condition) noexcept {
	EXPECT_FALSE(condition);
}

template<typename T>
void assertEqual(T expected, T actual) noexcept {
	EXPECT_EQ(expected, actual);
}

template<typename T>
void assertEqual(
	T expected,
	T actual,
	T tolerance
) noexcept {
	EXPECT_NEAR(expected, actual, tolerance);
}

void assertEqual(std::string expected, std::string actual) noexcept {
	assertEqual<std::string>(std::move(expected), std::move(actual));
}

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual
) {
	assertEqual<std::vector<T>::size_type>(expected.size(), actual.size());
	for (typename std::vector<T>::size_type i{ 0 }; i < expected.size(); ++i)
		assertEqual<T>(expected.at(i), actual.at(i));
}

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual,
	T tolerance
) {
	assertEqual<std::vector<T>::size_type>(expected.size(), actual.size());
	for (typename std::vector<T>::size_type i{ 0 }; i < expected.size(); ++i)
		assertEqual<T>(expected.at(i), actual.at(i), tolerance);
}

template void assertEqual(
	std::vector<float> expected,
	std::vector<float> actual,
	float tolerance
);

template void assertEqual(
	std::vector<float> expected,
	std::vector<float> actual
);

template void assertEqual(
	std::vector<double> expected,
	std::vector<double> actual
);

template void assertEqual(
	std::vector<double> expected,
	std::vector<double> actual,
	double tolerance
);

template void assertEqual(
	std::vector<std::string> expected,
	std::vector<std::string> actual
);

template void assertEqual(int, int) noexcept;
template void assertEqual(float, float) noexcept;
template void assertEqual(double, double) noexcept;
template void assertEqual(unsigned long, unsigned long) noexcept;
template void assertEqual(unsigned, unsigned) noexcept;
template void assertEqual(long long, long long) noexcept;
template void assertEqual(float *, float *) noexcept;