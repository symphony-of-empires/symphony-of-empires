#ifndef PRINT_HPP
#define PRINT_HPP

#ifdef _WIN32
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#include <stdio.h>
#define print_error(...)																	\
	fprintf(stderr, "[36m%s: \"%s\" %i:[0m ", __FILE__, __PRETTY_FUNCTION__, __LINE__);		\
	fprintf(stderr, __VA_ARGS__);															\
	fprintf(stderr, "\n");

#endif