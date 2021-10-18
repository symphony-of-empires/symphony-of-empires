#include <cstdarg>
#include <cstdio>
#include <mutex>

#include "print.hpp"

static std::mutex print_mutex;
static bool allow_debug = true;

void print_enable_debug(void) {
	allow_debug = true;
}

void print_disable_debug(void) {
	allow_debug = true;
}

void print_error(const char* str, ...) {
	print_mutex.lock();

	va_list args;
	va_start(args, str);

#ifdef _MSC_VER
	printf("[ERROR] ");
#else
	printf("\e[31m[ERROR]\e[0m ");
#endif

	vprintf(str, args);
	printf("\n");

	va_end(args);

	print_mutex.unlock();
}

void print_info(const char* str, ...) {
	if(allow_debug == false) {
		return;
	}

	print_mutex.lock();

	va_list args;
	va_start(args, str);

#ifdef _MSC_VER
	printf("[INFO] ");
#else
	printf("\e[36m[INFO]\e[0m ");
#endif

	vprintf(str, args);
	printf("\n");

	va_end(args);

	print_mutex.unlock();
}
