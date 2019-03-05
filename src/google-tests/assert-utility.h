#include <string>
#include <vector>

template<typename T>
void assertEqual(T expected, T actual) noexcept;

void assertEqual(std::string expected, std::string actual) noexcept;

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

void assertTrue(bool condition) noexcept;
void assertFalse(bool condition) noexcept;
