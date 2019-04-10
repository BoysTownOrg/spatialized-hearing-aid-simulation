#pragma once

#ifdef _WIN32
    #ifdef PLAYING_AUDIO_EXPORTS
        #define PLAYING_AUDIO_API __declspec(dllexport)
    #else
        #define PLAYING_AUDIO_API __declspec(dllimport)
    #endif
#else
    #define PLAYING_AUDIO_API
#endif
