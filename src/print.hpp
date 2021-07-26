#ifndef PRINT_HPP
#define PRINT_HPP

#include <sstream>
#include <cstdio>

/* mscvc does not support this extension of PRETTY_FUNCTION */
#ifdef _MSC_VER
#	define print_error(...)																\
    printf(__VA_ARGS__);																\
    printf("\n");
#	define print_info(...)																\
    printf(__VA_ARGS__);																\
    printf("\n");
/* mingw and gcc supports PRETTY_FUNCTION */
#else
#	define print_error(...)																\
    printf("\e[31m[ERROR]\e[0m ");	                                                    \
    printf(__VA_ARGS__);																\
    printf("\n");

#	define print_info(...)																\
    printf("\e[36m[INFO]\e[0m ");	                                                    \
    printf(__VA_ARGS__);																\
    printf("\n");
#endif

#endif
