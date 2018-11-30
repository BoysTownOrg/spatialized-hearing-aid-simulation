#pragma once

#ifdef AUDIO_STREAM_PROCESSING_EXPORTS
#define AUDIO_STREAM_PROCESSING_API __declspec(dllexport)
#else
#define AUDIO_STREAM_PROCESSING_API __declspec(dllimport)
#endif