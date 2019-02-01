#pragma once

#include <vector>

template<typename T>
class ArgumentCollection {
	std::vector<T> collection{};
public:
	bool contains(T item) {
		return std::find(collection.begin(), collection.end(), item) != collection.end();
	}

	auto push_back(T &&item) {
		return collection.push_back(item);
	}

	auto push_back(const T &item) {
		return collection.push_back(item);
	}

	auto empty() {
		return collection.empty();
	}

	auto at(typename std::vector<T>::size_type position) {
		return collection.at(position);
	}
};