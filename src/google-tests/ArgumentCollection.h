#pragma once

template<typename T>
class ArgumentCollection {
	std::vector<T> collection{};
public:
	bool contains(T item) {
		return std::find(collection.begin(), collection.end(), item) != collection.end();
	}

	auto push_back(T item) {
		return collection.push_back(std::move(item));
	}

	auto empty() {
		return collection.empty();
	}
};