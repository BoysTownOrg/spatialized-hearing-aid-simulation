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

	auto empty() const {
		return collection.empty();
	}

	auto at(typename std::vector<T>::size_type position) const {
		return collection.at(position);
	}
};