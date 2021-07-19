#include <stdexcept>
#ifdef unix
#	include <unistd.h>
#elif defined windows
#	ifndef _WINDOWS_
#		define WIN32_LEAN_AND_MEAN 1
#		include <windows.h>
#		undef WIN32_LEAN_AND_MEAN
#	endif
#endif
#include <climits>
#include <string>
#include <algorithm>
#include "path.hpp"

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
		throw std::runtime_error("Error reading exec path");
	}

	std::string get(std::string str) {
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
