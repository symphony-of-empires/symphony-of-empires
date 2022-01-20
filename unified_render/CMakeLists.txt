cmake_minimum_required(VERSION 3.5.1)

project(unified_render)
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
#set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

#
# Compilation flags
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS} -pthread")
IF(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dwindows=1")
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dunix=1")
ENDIF()
IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST=1")
ENDIF()
# Workaround for clang (see https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/SDL2")
link_directories(/usr/local/lib)
include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include")

#
# Source code files
#
file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_library(unified_render SHARED ${MAIN_SOURCES})

#
# Linking
#
target_link_libraries(unified_render PUBLIC SDL2 SDL2_ttf z tbb)
IF(WIN32)
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		
	ELSE()
		target_link_libraries(unified_render PUBLIC m stdc++ winpthread)
	ENDIF()
	target_link_libraries(unified_render PUBLIC glew32 opengl32 glu32 ws2_32 wsock32)
ELSE()
	target_link_libraries(unified_render PUBLIC m stdc++ GL GLU GLEW SDL2main)
ENDIF()

#
# Precompiled headers
#
# Standard C++ library headers
target_precompile_headers(unified_render PRIVATE
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
target_precompile_headers(unified_render PRIVATE
	<GL/glew.h>
	<GL/gl.h>
	<GL/glu.h>
)
# glm header files
target_precompile_headers(unified_render PRIVATE
	<glm/glm.hpp>
	<glm/mat4x4.hpp>
	<glm/common.hpp>
	<glm/vec2.hpp>
	<glm/vec3.hpp>
	<glm/gtx/intersect.hpp>
	<glm/gtc/type_ptr.hpp>
	<glm/gtc/matrix_transform.hpp>
)
