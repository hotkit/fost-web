/*
    Copyright 2014 Felspar Co Ltd. http://support.felspar.com/
    Distributed under the Boost Software License, Version 1.0.
    See accompanying file LICENSE_1_0.txt or copy at
        http://www.boost.org/LICENSE_1_0.txt
*/


#ifdef FOST_OS_WINDOWS
    #define PROXY_LIB_DECLSPEC __declspec( dllexport )
#else
    #define PROXY_LIB_DECLSPEC
#endif
