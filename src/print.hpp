#ifndef PRINT_HPP
#define PRINT_HPP

#include <sstream>
#include <cstdio>

/* mscvc does not support this extension of PRETTY_FUNCTION */
#ifdef _MSC_VER
#	define print_error(...)																\
    printf("%6s: %i: ", __FILE__, __LINE__);											\
    printf(__VA_ARGS__);																\
    printf("\n");
#	define print_info(...)																\
    printf("%6s: %i: ", __FILE__, __LINE__);											\
    printf(__VA_ARGS__);																\
    printf("\n");
/* mingw and gcc supports PRETTY_FUNCTION */
#else
#	define print_error(...)																\
    printf("\e[31m%6s: \"%8s\" %i:\e[0m ", __FILE__, __PRETTY_FUNCTION__, __LINE__);	\
    printf(__VA_ARGS__);																\
    printf("\n");

#	define print_info(...)																\
    printf("\e[36m%6s: \"%8s\" %i:\e[0m ", __FILE__, __PRETTY_FUNCTION__, __LINE__);	\
    printf(__VA_ARGS__);																\
    printf("\n");
#endif

#endif
