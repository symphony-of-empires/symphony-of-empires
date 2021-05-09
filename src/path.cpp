#include <string>
#include <iostream>
#ifndef WIN32
#include <unistd.h>
#endif
#include <limits.h>

#ifdef WIN32
#	include <Libloaderapi.h>
#endif

static inline std::string Resource_GetSelfPath(){
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
	std::cerr << "ERROR: Failed to get path to self" << std::endl;
	exit(EXIT_FAILURE);
}

std::string Resource_GetPath(const char * str){
	if(str[0] == '/' || str[0] == 'C') return str;
	std::string rsult = Resource_GetSelfPath();
	size_t found = rsult.find_last_of("/\\");
	rsult = rsult.substr(0, found);
	found = rsult.find_last_of("/\\");
	rsult = rsult.substr(0, found);
	rsult += "/data/";
	rsult += str;
	std::cout << "Concat str: " << rsult << std::endl;
	return rsult;
}

