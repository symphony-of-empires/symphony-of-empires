#ifndef PRINT_HPP
#define PRINT_HPP
#include <sstream>
#include <cstdio>

void print_enable_debug(void);
void print_disable_debug(void);
void print_error(const char* str, ...);
void print_info(const char* str, ...);

#endif
