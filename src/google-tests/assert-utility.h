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

inline void assertEqual(std::string expected, std::string actual) {
	EXPECT_EQ(expected, actual);
}
