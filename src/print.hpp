#ifndef PRINT_HPP
#define PRINT_HPP

#include <stdio.h>
#define print_error(...)																	\
	printf("\e[31m%s: \"%s\" %i:\e[0m ", __FILE__, __PRETTY_FUNCTION__, __LINE__);		\
	printf(__VA_ARGS__);															\
	printf("\n");

#define print_info(...)																	\
	printf("\e[36m%s: \"%s\" %i:\e[0m ", __FILE__, __PRETTY_FUNCTION__, __LINE__);		\
	printf(__VA_ARGS__);															\
	printf("\n");

#endif
