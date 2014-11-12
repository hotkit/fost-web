#ifdef FOST_OS_WINDOWS
    #define PROXY_LIB_DECLSPEC __declspec( dllexport )
#else
    #define PROXY_LIB_DECLSPEC
#endif
