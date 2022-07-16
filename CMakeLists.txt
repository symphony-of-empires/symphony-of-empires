cmake_minimum_required(VERSION 3.15)
project(SymphonyOfEmpires CXX C)
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

option(BUILD_ENGINE "Enable building Eng3D" ON)
option(BUILD_GAME "Enable building SymphonyOfEmpires" ON)

IF(WIN32)
	add_definitions( -DNOMINMAX=1 )
	add_definitions( -DWIN32_LEAN_AND_MEAN=1 )
ENDIF()

IF(ANDROID)
	set(NDK_PROJECT_PATH $ENV{PWD}) # Set project path to current directory
	set(ANDROID_STL c++_static) # Static libc++
	IF(NOT DEFINED ANDROID_HOME)
		message(FATAL_ERROR "Please define ANDROID_NDK_HOME")
	ENDIF()
	IF(NOT DEFINED ANDROID_NDK_HOME)
		message(FATAL_ERROR "Please define ANDROID_NDK_HOME")
	ENDIF()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -fPIC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE -fPIC")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
ENDIF()

IF(NO_COPY_MODS)
	add_compile_definitions(NO_COPY_MODS)
ENDIF()
IF(SANITIZE_ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=thread")
ENDIF()

# Backend selection
IF(E3D_BACKEND_OPENGL)
	message("Using OpenGL backend")
	IF(ANDROID)
		message(FATAL_ERROR "Use E3D_BACKEND_GLES instead on Android")
	ENDIF()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_BACKEND_OPENGL=1")
ELSEIF(E3D_BACKEND_GLES)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_BACKEND_GLES=1")
ELSEIF(E3D_BACKEND_D3D9)
	message("Using DirectX 9 SDK")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_BACKEND_D3D9=1")
ELSE()
	message(FATAL_ERROR "No backend specified!")
ENDIF()

# Build stuff
IF(BUILD_ENGINE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/eng3d ${CMAKE_BINARY_DIR}/eng3d)
ENDIF()

#
# Dependencies
#
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_Declare(zlib GIT_REPOSITORY "https://github.com/madler/zlib" GIT_TAG 21767c654d31d2dccdde4330529775c6c5fd5389 GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(zlib)
IF(0)
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
ENDIF()
IF(0)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_IGNORE_GIT_HASH ON)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(BUILD_LIB_TYPE STATIC)
set(ENABLE_BOOST_WORKAROUND ON)
FetchContent_Declare(assimp GIT_REPOSITORY "https://github.com/assimp/assimp" GIT_TAG v5.2.4 GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(assimp)
ENDIF()
set(LUA_BUILD_BINARY OFF)
set(LUA_BUILD_COMPILER OFF)
set(LUA_ENABLE_SHARED ON)
set(LUA_SUPPORT_DL ON)
FetchContent_Declare(lua GIT_REPOSITORY "https://github.com/walterschell/Lua" GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(lua)
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm" GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(glm)
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
set(LUA_BUILD_BINARY OFF)
set(LUA_BUILD_COMPILER OFF)
FetchContent_Declare(lua GIT_REPOSITORY "https://github.com/walterschell/Lua" GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(lua)
include_directories("${lua_SOURCE_DIR}/lua-5.4.4/include")

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
	link_directories("${DEVKITPRO}/libogc/lib")
	include_directories("${DEVKITPRO}/libogc/include")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_UNIX=1")
ENDIF()
IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
ENDIF()

include_directories("${PROJECT_SOURCE_DIR}/game/src" "${CMAKE_SOURCE_DIR}/eng3d")
include_directories("/usr/include/SDL2")

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
IF(ANDROID)
	add_executable(SymphonyOfEmpires "${MAIN_SOURCES}" "${ANDROID_NDK_HOME}/sources/android/native_app_glue/android_native_app_glue.c")
ELSE()
	add_executable(SymphonyOfEmpires "${MAIN_SOURCES}")
ENDIF()

#
# Linking
#

# Pthread
find_package(Threads)
IF(Threads_FOUND)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	target_link_libraries(SymphonyOfEmpires PRIVATE Threads::Threads)
ENDIF()

# Platform-specific libraries
IF(WIN32)
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
		target_link_libraries(eng3d PRIVATE libglew_static)
	ELSE()
		target_link_libraries(SymphonyOfEmpires PRIVATE m stdc++)
	ENDIF()
	target_link_libraries(SymphonyOfEmpires PRIVATE ws2_32 wsock32)
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(SymphonyOfEmpires PRIVATE opengl32)
	ENDIF()
ELSEIF(UNIX OR ANDROID)
	target_link_libraries(SymphonyOfEmpires PRIVATE m stdc++)
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(SymphonyOfEmpires PRIVATE GL GLEW)
	ELSEIF(E3D_BACKEND_GLES)
		target_link_libraries(SymphonyOfEmpires PRIVATE EGL GLESv1_CM GLESv2 GLESv3)
	ENDIF()
ENDIF()
target_link_libraries(SymphonyOfEmpires PUBLIC eng3d SDL2)

set(APP_SHARED_LIBRARIES
	${LIBRARY_OUTPUT_PATH}/libtbb.so
	${LIBRARY_OUTPUT_PATH}/libassimp.so
	${LIBRARY_OUTPUT_PATH}/libSDL2.so
	${LIBRARY_OUTPUT_PATH}/libSDL2_ttf.so
	${LIBRARY_OUTPUT_PATH}/liblua.so
	${LIBRARY_OUTPUT_PATH}/libeng3d.so
)
