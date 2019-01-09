#pragma once

#ifdef STIMULUS_LIST_EXPORTS
	#define STIMULUS_LIST_API __declspec(dllexport)
#else
	#define STIMULUS_LIST_API __declspec(dllimport)
#endif