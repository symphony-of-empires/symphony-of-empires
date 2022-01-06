cmake_minimum_required(VERSION 3.5.1)

project(game)
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
#set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

#
# Compilation flags
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS} -pthread")
IF(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dwindows")
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dunix")
ENDIF()
IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
ENDIF()
# Workaround for clang (see https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/SDL2")
link_directories(/usr/local/lib)
include_directories("${PROJECT_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/unified_render/include")

#
# Source code files
#

# C++ source files
file(GLOB MAIN_SOURCES
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/client/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/client/interface/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/client/ui/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/server/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/server/ai/*.cpp"
)
add_executable(game ${MAIN_SOURCES})

#
# Linking
#
IF(WIN32)
	target_link_libraries(game PUBLIC opengl32 glu32 winpthread)
ELSE()
	target_link_libraries(game PUBLIC GL GLU SDL2main)
ENDIF()

# Lua library is quite strange and requires some conditionals to be able to compile it
IF(NOT MINGW AND NOT MSYS)
	IF(lua54)
		message(STATUS "Using lua v5.4")
		target_link_libraries(game PUBLIC lua5.4)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA54")
	ELSE()
		message(STATUS "Using lua v5.3")
		target_link_libraries(game PUBLIC lua5.3)
	ENDIF()
# msys is simple - just lua, no lua5.4 or lua5.3
ELSE()
	message(STATUS "Using an unknown lua version")
	target_link_libraries(game PUBLIC lua)
ENDIF()
target_link_libraries(game PUBLIC SDL2 m z tbb stdc++ unified_render)

#
# Precompiled headers
#
# Standard C++ library headers
target_precompile_headers(game PRIVATE
	<cstdint>
	<cstddef>
	<cstring>
	<cstdlib>
	<cstdio>
	<climits>
	<ctime>
	<sstream>
	<fstream>
	<string>
	<vector>
	<set>
	<deque>
	<atomic>
	<thread>
	<mutex>
	<utility>
	<numeric>
	<functional>
	<type_traits>
	<algorithm>
	<memory>
	<iostream>
	<unordered_map>
)
# OpenGL header files
target_precompile_headers(game PRIVATE
	<GL/glew.h>
	<GL/gl.h>
	<GL/glu.h>
	<GL/glext.h>
)
# glm header files
target_precompile_headers(game PRIVATE
	<glm/glm.hpp>
	<glm/mat4x4.hpp>
	<glm/common.hpp>
	<glm/vec2.hpp>
	<glm/vec3.hpp>
	<glm/gtx/intersect.hpp>
	<glm/gtc/type_ptr.hpp>
	<glm/gtc/matrix_transform.hpp>
)