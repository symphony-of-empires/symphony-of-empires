cmake_minimum_required(VERSION 3.5.1)

project(unified_render)
#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
#set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# Workaround for clang (see https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023)
include_directories(/usr/include/SDL2)

link_directories(/usr/local/lib)
include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include")

# TODO: Windows specific
link_directories(${CMAKE_SOURCE_DIR}/vendor/lib)
include_directories(${CMAKE_SOURCE_DIR}/vendor/include)

#
# Source code files
#
file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/ui/*.cpp")
add_library(unified_render STATIC ${MAIN_SOURCES})

#
# Linking
#
target_link_libraries(unified_render PUBLIC SDL2 SDL2_ttf z tbb)
IF(WIN32)
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
		
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
