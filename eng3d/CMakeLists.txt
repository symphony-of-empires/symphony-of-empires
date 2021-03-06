cmake_minimum_required(VERSION 3.15)

project(eng3d CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

option(E3D_BUILD_ENGINE "Enable building the Eng3D library" ON)

#
# Defines - see README.1st
#
option(E3D_DEBUG, "Enable debug" ON)
option(E3D_BACKEND_OPENGL, "Enable the OpenGL backend" ON)
option(E3D_BACKEND_GLES, "Enable the GLES backend" ON)
option(E3D_BACKEND_RGX, "E3D_BACKEND_RGX" ON)
option(E3D_MANAGED_HEAP, "Allow Eng3D to manage heap (useful for debugging without instrumentation)" ON)
option(E3D_VENDORED, "Build libraries instead of using the system ones" OFF)

IF(E3D_DEBUG)
	add_definitions(-DE3D_DEBUG=1)
ENDIF()
IF(E3D_BACKEND_OPENGL)
	add_definitions(-DE3D_BACKEND_OPENGL=1)
ENDIF()
IF(E3D_BACKEND_GLES)
	add_definitions(-DE3D_BACKEND_GLES=1)
ENDIF()
IF(E3D_BACKEND_RGX)
	add_definitions(-DE3D_BACKEND_RGX=1)
ENDIF()
IF(E3D_MANAGED_HEAP)
	add_definitions(-DE3D_MANAGED_HEAP=1)
ENDIF()

#
# Compilation flags
#
IF(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS}")
ENDIF()
IF(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_WINDOWS=1")
ELSEIF(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_UNIX=1")
ELSEIF(ANDROID)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_UNIX=1 -DE3D_TARGET_ANDROID=1")
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_RVL=1")
ENDIF()

IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST=1")
ENDIF()

IF(NOT E3D_TARGET_RVL)
	# Workaround for clang (see https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023)
	link_directories(/usr/local/lib)
ELSE()

ENDIF()
include_directories("${PROJECT_SOURCE_DIR}")

#
# Dependencies
#
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
IF(E3D_VENDORED)
	# GLEW
	IF(WIN32)
		set(ONLY_LIBS ON)
		set(glew-cmake_BUILD_SHARED ON)
		FetchContent_Declare(glew-cmake GIT_REPOSITORY "https://github.com/Perlmint/glew-cmake" GIT_PROGRESS TRUE)
		FetchContent_MakeAvailable(glew-cmake)
		include_directories("${glew-cmake_SOURCE_DIR}/include")
	ENDIF()
	# SDL2
	set(SDL2_DISABLE_SDL2MAIN ON)
	set(SDL2_DISABLE_INSTALL ON)
	set(SDL2_DISABLE_UNINSTALL ON)
	set(SDL_STATIC_PIC ON)
	set(SDL_TEST OFF)
	IF(UNIX)
		set(SDL_X11_XCURSOR ON)
		set(SDL_X11_XINPUT ON)
		set(SDL_X11_XRANDR ON)
		set(SDL_X11_XFIXES ON)
		set(SDL_X11_XSCRNSAVER ON)
		set(SDL_X11_XSHAPE ON)
		set(SDL_X11_XDBE ON)
		set(SDL_VIDEO_DRIVER_X11_SUPPORTS_GENERIC_EVENTS 1)
		set(SDL_VIDEO_DRIVER_X11_CONST_PARAM_XEXTADDDISPLAY 1)
	ENDIF()
	FetchContent_Declare(sdl2 GIT_REPOSITORY "https://github.com/libsdl-org/SDL" GIT_TAG main GIT_PROGRESS TRUE)
	FetchContent_MakeAvailable(sdl2)
	include_directories("${sdl2_BINARY_DIR}/include-config-debug" "${sdl2_BINARY_DIR}/include-config-release" "${sdl2_BINARY_DIR}/include-config-relwithdebinfo" "${sdl2_BINARY_DIR}/include")
	# SDL2_TTF
	set(SDL2TTF_VENDORED ON)
	set(SDL2TTF_INSTALL OFF)
	set(SDL2TTF_SAMPLES OFF)
	FetchContent_Declare(sdl2_ttf GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf" GIT_TAG main GIT_PROGRESS TRUE)
	FetchContent_MakeAvailable(sdl2_ttf)
	include_directories("${sdl2_ttf_SOURCE_DIR}")
	# ASSIMP
	set(ASSIMP_INSTALL OFF)
	set(ASSIMP_NO_EXPORT ON)
	set(ASSIMP_IGNORE_GIT_HASH ON)
	set(ASSIMP_WARNINGS_AS_ERRORS OFF)
	set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
	set(ASSIMP_BUILD_SAMPLES OFF)
	set(ASSIMP_BUILD_TESTS OFF)
	set(ASSIMP_BUILD_ZLIB OFF)
	set(BUILD_LIB_TYPE STATIC)
	set(ENABLE_BOOST_WORKAROUND ON)
	FetchContent_Declare(assimp GIT_REPOSITORY "https://github.com/assimp/assimp" GIT_TAG v5.2.4 GIT_PROGRESS TRUE)
	FetchContent_MakeAvailable(assimp)
	# TBB (and hwloc if needed)
	IF(WIN32)
		FetchContent_Declare(hwloc URL "https://download.open-mpi.org/release/hwloc/v2.7/hwloc-win64-build-2.7.1.zip")
		FetchContent_MakeAvailable(hwloc)
		set(CMAKE_HWLOC_2_5_DLL_PATH "${hwloc_SOURCE_DIR}/lib/libhwloc.dll.a")
		set(CMAKE_HWLOC_2_5_LIBRARY_PATH "${hwloc_SOURCE_DIR}/lib/libhwloc.lib" )
		set(CMAKE_HWLOC_2_5_INCLUDE_PATH "${hwloc_SOURCE_DIR}/include" )
	ENDIF()
	set(TBB_TEST OFF)
	set(TBB_EXAMPLES OFF)
	set(TBB_ENABLE_IPO OFF)
	set(TBB_CPF OFF)
	FetchContent_Declare(opentbb GIT_REPOSITORY "https://github.com/oneapi-src/oneTBB" GIT_TAG 3df08fe234f23e732a122809b40eb129ae22733f GIT_PROGRESS TRUE)
	FetchContent_MakeAvailable(opentbb)
	# GLFW
	IF(NOT ANDROID)
		IF(E3D_BACKEND_OPENGL)
			set(GLFW_LIBRARY_TYPE STATIC)
			set(GLFW_BUILD_TESTS OFF)
			set(GLFW_BUILD_EXAMPLES OFF)
			set(GLFW_BUILD_DOCS OFF)
			set(GLFW_INSTALL OFF)
			FetchContent_Declare(glfw GIT_REPOSITORY "https://github.com/glfw/glfw")
			FetchContent_MakeAvailable(glfw)
		ENDIF()
	ENDIF()
ENDIF()

# ZLIB
FetchContent_Declare(zlib GIT_REPOSITORY "https://github.com/madler/zlib" GIT_TAG 21767c654d31d2dccdde4330529775c6c5fd5389 GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(zlib)
# GLM
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm" GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(glm)
# LUA
set(LUA_BUILD_BINARY OFF)
set(LUA_BUILD_COMPILER OFF)
set(LUA_ENABLE_SHARED ON)
set(LUA_SUPPORT_DL ON)
FetchContent_Declare(lua GIT_REPOSITORY "https://github.com/walterschell/Lua" GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(lua)
include_directories("${lua_SOURCE_DIR}/lua-5.4.4/include")

#
# Source code files
#
file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/ui/*.cpp")
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/eng3d")
add_library(eng3d SHARED ${MAIN_SOURCES})

IF(NOT E3D_VENDORED)
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIRS})
	target_link_libraries(eng3d PRIVATE ${SDL2_LIBRARIES})
ENDIF()

#
# Linking
#
target_link_libraries(eng3d PUBLIC glm tbb lua_static)
target_link_libraries(eng3d PRIVATE SDL2_ttf assimp)
IF(WIN32)
	target_link_libraries(eng3d PRIVATE ws2_32 wsock32)
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
		target_link_libraries(eng3d PRIVATE libglew_static zlib)
	ELSE()
		target_link_libraries(eng3d PRIVATE m stdc++)
	ENDIF()
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(eng3d PRIVATE opengl32)
	ENDIF()
ELSEIF(UNIX OR ANDROID)
	target_link_libraries(eng3d PRIVATE m stdc++ zlibstatic)
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(eng3d PRIVATE GL GLEW)
	ELSEIF(E3D_BACKEND_GLES)
		target_link_libraries(eng3d PRIVATE EGL GLESv1_CM GLESv2 GLESv3)
	ENDIF()
ENDIF()

# Pthread
find_package(Threads)
IF(Threads_FOUND)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	target_link_libraries(eng3d PRIVATE Threads::Threads)
ENDIF()
