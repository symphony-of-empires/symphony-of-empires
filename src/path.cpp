#include "path.hpp"
#ifdef unix
#	include <unistd.h>
#elif defined windows
#	include <windows.h>
#endif
#include <limits.h>
#include <string>
#include <algorithm>

namespace Path {
	static inline std::string get_exec_path(void) {
#ifdef windows
		char buf[PATH_MAX];
		const auto len = GetModuleFileNameA(nullptr, buf, sizeof(buf) - 1);
#else
		char buf[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
#endif
		if(len != -1) {
			buf[len] = '\0';
			return std::string(buf);
		}
		exit(EXIT_FAILURE);
	}

	std::string get(const char* str) {
		if(str[0] == '/' || str[0] == 'C')
			return str;
		
		std::string rsult = get_exec_path();
		size_t found = rsult.find_last_of("/\\");
		rsult = rsult.substr(0, found);
		found = rsult.find_last_of("/\\");
		rsult = rsult.substr(0, found);
		rsult += "/mods/base_game/";
		rsult += str;
#ifdef windows
		std::replace(rsult.begin(), rsult.end(), '/', '\\');
#endif
		return rsult;
	}
};
