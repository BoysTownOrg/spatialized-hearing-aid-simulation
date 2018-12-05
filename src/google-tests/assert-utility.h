#include <gtest/gtest.h>
#include <string>

inline void assertEqual(std::string expected, std::string actual) {
	EXPECT_EQ(expected, actual);
}

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual)
{
	using size_type = typename std::vector<T>::size_type;
	EXPECT_EQ(expected.size(), actual.size());
	for (size_type i = 0; i < expected.size(); ++i)
		EXPECT_EQ(expected.at(i), actual.at(i));
}
