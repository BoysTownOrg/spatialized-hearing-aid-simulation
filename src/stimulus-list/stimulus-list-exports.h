#pragma once

#ifdef _WIN32
    #ifdef STIMULUS_LIST_EXPORTS
        #define STIMULUS_LIST_API __declspec(dllexport)
    #else
        #define STIMULUS_LIST_API __declspec(dllimport)
    #endif
#else
    #define STIMULUS_LIST_API
#endif
