#pragma once

#ifdef _WIN32
    #ifdef AUDIO_FILE_READING_WRITING_EXPORTS
        #define AUDIO_FILE_READING_WRITING_API __declspec(dllexport)
    #else
        #define AUDIO_FILE_READING_WRITING_API __declspec(dllimport)
    #endif
#else
    #define AUDIO_FILE_READING_WRITING_API
#endif
