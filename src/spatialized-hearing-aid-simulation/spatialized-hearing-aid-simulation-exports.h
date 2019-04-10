#pragma once

#ifdef _WIN32
    #ifdef SPATIALIZED_HA_SIMULATION_EXPORTS
        #define SPATIALIZED_HA_SIMULATION_API __declspec(dllexport)
    #else
        #define SPATIALIZED_HA_SIMULATION_API __declspec(dllimport)
    #endif
#else
    #define SPATIALIZED_HA_SIMULATION_API
#endif
