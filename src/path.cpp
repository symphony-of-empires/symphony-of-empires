#include <string>
#include <iostream>
#include <unistd.h>
#include <limits.h>

std::string getPathToSelf(){
	char buf[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    	if (len != -1) {
      		buf[len] = '\0';
      		return std::string(buf);
    	}
	std::cerr << "ERROR: Failed to get path to self" << std::endl;
	exit(EXIT_FAILURE);
}

std::string Resource_GetPath(const char * str){
	if(str[0] == '/' || str[0] == 'c')
		return str;
	std::string rsult = getPathToSelf();
	rsult.erase(rsult.end() - 4, rsult.end());
	rsult += "../data/";
	rsult += str;
	std::cout << "Concat str: " << rsult << std::endl;
	return rsult;
}

