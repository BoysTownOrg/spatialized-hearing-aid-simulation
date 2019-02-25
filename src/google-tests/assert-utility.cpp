#include "assert-utility.h"
#include <gtest/gtest.h>

void assertEqual(std::string expected, std::string actual) {
	EXPECT_EQ(expected, actual);
}

void assertTrue(bool condition) {
	EXPECT_TRUE(condition);
}

void assertFalse(bool condition) {
	EXPECT_FALSE(condition);
}

template<typename T>
void assertEqual(T expected, T actual) {
	EXPECT_EQ(expected, actual);
}

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual
) {
	EXPECT_EQ(expected.size(), actual.size());
	for (typename std::vector<T>::size_type i{ 0 }; i < expected.size(); ++i)
		EXPECT_EQ(expected.at(i), actual.at(i));
}

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual,
	T tolerance
) {
	EXPECT_EQ(expected.size(), actual.size());
	for (typename std::vector<T>::size_type i{ 0 }; i < expected.size(); ++i)
		EXPECT_NEAR(expected.at(i), actual.at(i), tolerance);
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
	std::vector<std::string> expected,
	std::vector<std::string> actual
);

template void assertEqual(int, int);
template void assertEqual(float, float);
template void assertEqual(double, double);
template void assertEqual(unsigned long, unsigned long);
template void assertEqual(unsigned, unsigned);
template void assertEqual(long long, long long);
template void assertEqual(float *, float *);