#include "assert-utility.h"
#include <gtest/gtest.h>
#include <string>

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual)
{
	EXPECT_EQ(expected.size(), actual.size());
	for (std::size_t i = 0; i < expected.size(); ++i)
		EXPECT_EQ(expected.at(i), actual.at(i));
}

template void assertEqual(
	std::vector<std::string> expected,
	std::vector<std::string> actual);

template void assertEqual(
	std::vector<float> expected,
	std::vector<float> actual);
