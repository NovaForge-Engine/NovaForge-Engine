#pragma once


#if defined(_MSC_VER)
    //  Microsoft 
    #ifdef FORGE_EXPORT
        #define ENGINE_DLL __declspec(dllexport)
    #else
        #define ENGINE_DLL __declspec(dllimport)
    #endif
#elif defined(__clang__)
    //  GCC
    #ifdef FORGE_EXPORT
        // "default" here means "visible when in shared lib"
        // This is the default behaviour for GCC and Clang, contrary to MSVC
        #define ENGINE_DLL __attribute__((__visibility__("default")))
    #else
        #define ENGINE_DLL
    #endif
#elif defined(__GNUC)
    //  GCC
    #ifdef FORGE_EXPORT
        // "default" here means "visible when in shared lib"
        // This is the default behaviour for GCC and Clang, contrary to MSVC
        #define ENGINE_DLL attribute((visibility("default")))
    #else
        #define ENGINE_DLL
    #endif
#else
    //  do nothing and hope for the best?
    #define ENGINE_DLL
    #pragma warning Unknown dynamic link import/export semantics.
#endif
