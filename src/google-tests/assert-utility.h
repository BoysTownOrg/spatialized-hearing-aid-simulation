#include <string>
#include <vector>

template<typename T>
void assertEqual(T expected, T actual);

void assertEqual(std::string expected, std::string actual);

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual
);

template<typename T>
void assertEqual(
	std::vector<T> expected,
	std::vector<T> actual,
	T tolerance
);

void assertTrue(bool condition);
void assertFalse(bool condition);
