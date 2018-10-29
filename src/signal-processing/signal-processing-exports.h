#pragma once

#ifdef SIGNAL_PROCESSING_EXPORTS
	#define SIGNAL_PROCESSING_API __declspec(dllexport)
#else
	#define SIGNAL_PROCESSING_API __declspec(dllimport)
#endif
