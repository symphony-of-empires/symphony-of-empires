#include "path.hpp"
#include <filesystem>

#ifdef _WIN32
	#include <windows.h>
	#include <Libloaderapi.h>

	constexpr size_t PATH_MAX = 256;
#else
	#include <unistd.h>
	#include <limits.h>
#endif

namespace fs = std::filesystem;

namespace Path {
	static inline std::string get_exec_path(void) {
	#ifdef WIN32
		char buf[PATH_MAX];
		const auto len = GetModuleFileNameA(nullptr, buf, sizeof(buf) - 1);
	#else
		char buf[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
	#endif
		if (len != -1) {
			buf[len] = '\0';
			return std::string(buf);
		}
		exit(EXIT_FAILURE);
	}

	std::string get(const char * str) {
		if(fs::path(str).is_absolute()) {
			return str;
		}

		auto strPath = fs::path(str);
		auto mainFolder = fs::path(get_exec_path()).parent_path();

		mainFolder /= "data";

		for(auto slice : strPath)
		{
			mainFolder /= slice;
		}

		return mainFolder.string();
	}
};