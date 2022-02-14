cmake_minimum_required(VERSION 3.5.1)

project(SymphonyOfEmpires)

option(BUILD_ENGINE "Enable building UnifiedRender" ON)
option(BUILD_GAME "Enable building SymphonyOfEmpires" ON)

# Backend selection
IF(UR_BACKEND_OPENGL)
	message("Using OpenGL backend")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUR_BACKEND_OPENGL=1")
ELSE()
	IF(UR_BACKEND_D3D9)
		message("Using DirectX 9 SDK")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUR_BACKEND_D3D9=1")
	ELSE()
		
	ENDIF()
ENDIF()

# Build stuff
if(BUILD_ENGINE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/unified_render)
endif()

#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
#set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

#
# Compilation flags
#
set(CMAKE_CXX_STANDARD 20)

IF(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS} -pthread")
ENDIF()
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
include_directories("${PROJECT_SOURCE_DIR}/game/src" "${CMAKE_SOURCE_DIR}/unified_render/include")

link_directories(${PROJECT_SOURCE_DIR}/external/lib)
include_directories(${PROJECT_SOURCE_DIR}/external/include)

#
# Source code files
#

# C++ source files
file(GLOB MAIN_SOURCES
	"${PROJECT_SOURCE_DIR}/game/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/game/src/client/*.cpp"
	"${PROJECT_SOURCE_DIR}/game/src/client/interface/*.cpp"
	"${PROJECT_SOURCE_DIR}/game/src/server/*.cpp"
	"${PROJECT_SOURCE_DIR}/game/src/server/ai/*.cpp"
)
add_executable(SymphonyOfEmpires ${MAIN_SOURCES})

#
# Linking
#
target_link_libraries(SymphonyOfEmpires PUBLIC SDL2 z unified_render tbb)
IF(WIN32)
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
		
	ELSE()
		target_link_libraries(SymphonyOfEmpires PUBLIC stdc++ m winpthread)
	ENDIF()
	target_link_libraries(SymphonyOfEmpires PUBLIC opengl32 glu32)
ELSE()
	target_link_libraries(SymphonyOfEmpires PUBLIC m stdc++ GL GLU SDL2main)
ENDIF()

# Lua library is quite strange and requires some conditionals to be able to compile it

IF(NOT MINGW AND NOT MSYS)
	IF(lua54)
		message(STATUS "Using lua v5.4")
		IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
			target_link_libraries(SymphonyOfEmpires PUBLIC lua54)
		ELSE()
			target_link_libraries(SymphonyOfEmpires PUBLIC lua5.4)
		ENDIF()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLUA54")
	ELSE()
		message(STATUS "Using lua v5.3")
		
		IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
			target_link_libraries(SymphonyOfEmpires PUBLIC lua53)
		ELSE()
			target_link_libraries(SymphonyOfEmpires PUBLIC lua5.3)
		ENDIF()
	ENDIF()
# msys is simple - just lua, no lua5.4 or lua5.3
ELSE()
	message(STATUS "Using an unknown lua version")
	target_link_libraries(SymphonyOfEmpires PUBLIC lua)
ENDIF()

#
# Precompiled headers
#
# Standard C++ library headers
target_precompile_headers(SymphonyOfEmpires PRIVATE
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
target_precompile_headers(SymphonyOfEmpires PRIVATE
	<GL/glew.h>
	<GL/gl.h>
	<GL/glu.h>
	<GL/glext.h>
)
# glm header files
target_precompile_headers(SymphonyOfEmpires PRIVATE
	<glm/glm.hpp>
	<glm/mat4x4.hpp>
	<glm/common.hpp>
	<glm/vec2.hpp>
	<glm/vec3.hpp>
	<glm/gtx/intersect.hpp>
	<glm/gtc/type_ptr.hpp>
	<glm/gtc/matrix_transform.hpp>
)