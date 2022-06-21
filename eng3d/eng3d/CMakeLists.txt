cmake_minimum_required(VERSION 3.5.1)

project(eng3d)
set(CMAKE_CXX_STANDARD 20)

IF(NOT E3D_TARGET_RVL)
	# Workaround for clang (see https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023)
	link_directories(/usr/local/lib)
ELSE()

ENDIF()
include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include")

#
# Source code files
#
file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/ui/*.cpp")
add_library(eng3d STATIC ${MAIN_SOURCES})

#
# Linking
#
#add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/assimp)
include_directories(${CMAKE_SOURCE_DIR}/vendor/assimp/include)
#target_link_libraries(eng3d PUBLIC Assimp)
#add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glm)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)
#target_link_libraries(eng3d PUBLIC glm)
#add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/onetbb)
include_directories(${CMAKE_SOURCE_DIR}/vendor/onetbb/include)
#target_link_libraries(eng3d PUBLIC TBB)
#add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/sdl2)
include_directories(${CMAKE_SOURCE_DIR}/vendor/sdl2/include)
#target_link_libraries(eng3d PUBLIC SDL2)
#add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/lua)
include_directories(${CMAKE_SOURCE_DIR}/vendor/lua)
#target_link_libraries(eng3d PUBLIC lua)
#add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/sdl2_ttf)
include_directories(${CMAKE_SOURCE_DIR}/vendor/sdl2_ttf)
#target_link_libraries(eng3d PUBLIC sdl2_ttf)

IF(WIN32)
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
		
	ELSE()
		target_link_libraries(eng3d PUBLIC m stdc++ winpthread)
	ENDIF()
	target_link_libraries(eng3d PUBLIC ws2_32 wsock32)
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(eng3d PUBLIC glew32 opengl32 glu32)
	ENDIF()
ELSEIF(UNIX OR ANDROID)
	target_link_libraries(eng3d PUBLIC m stdc++)
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(eng3d PUBLIC GLEW GL GLU)
	ELSEIF(E3D_BACKEND_GLES)
		target_link_libraries(eng3d PUBLIC EGL GLESv1_CM GLESv2 GLESv3)
	ENDIF()
ELSE()

ENDIF()
target_link_libraries(eng3d PUBLIC z)

# Find pthreads
find_package(Threads)
IF(Threads_FOUND)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	target_link_libraries(eng3d PRIVATE Threads::Threads)
ENDIF()

#
# Precompiled headers
#
# Standard C++ library headers
target_precompile_headers(eng3d PRIVATE
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

IF(E3D_BACKEND_OPENGL)
	# OpenGL header files
	target_precompile_headers(eng3d PRIVATE
		<GL/glew.h>
		<GL/gl.h>
		<GL/glu.h>
	)
ELSEIF(E3D_BACKEND_GLES)
	# No precompile
ENDIF()

# glm header files
target_precompile_headers(eng3d PRIVATE
	<glm/glm.hpp>
	<glm/mat4x4.hpp>
	<glm/common.hpp>
	<glm/vec2.hpp>
	<glm/vec3.hpp>
	<glm/gtx/intersect.hpp>
	<glm/gtc/type_ptr.hpp>
	<glm/gtc/matrix_transform.hpp>
)
