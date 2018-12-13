#include <gtest/gtest.h>
#include <string>

inline void assertEqual(std::string expected, std::string actual) {
	EXPECT_EQ(expected, actual);
}

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual
) {
	EXPECT_EQ(expected.size(), actual.size());
	using size_type = typename decltype(expected)::size_type;
	for (size_type i = 0; i < expected.size(); ++i)
		EXPECT_EQ(expected.at(i), actual.at(i));
}

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual,
	T tolerance
) {
	EXPECT_EQ(expected.size(), actual.size());
	using size_type = typename decltype(expected)::size_type;
	for (size_type i = 0; i < expected.size(); ++i)
		EXPECT_NEAR(expected.at(i), actual.at(i), tolerance);
}
