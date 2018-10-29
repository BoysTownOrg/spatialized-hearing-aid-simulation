#pragma once

#ifdef AUDIO_PROCESSING_EXPORTS
	#define AUDIO_PROCESSING_API __declspec(dllexport)
#else
	#define AUDIO_PROCESSING_API __declspec(dllimport)
#endif