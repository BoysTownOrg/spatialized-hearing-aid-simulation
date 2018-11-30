#pragma once

#ifdef BRIR_CONFIG_EXPORTS
#define BRIR_CONFIG_API __declspec(dllexport)
#else
#define BRIR_CONFIG_API __declspec(dllimport)
#endif

#include <string>

namespace brir_config {
	enum class Property {
		leftImpulseResponse,
		rightImpulseResponse,
		sampleRate
	};

	BRIR_CONFIG_API std::string propertyName(Property);
}