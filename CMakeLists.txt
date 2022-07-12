cmake_minimum_required(VERSION 3.14)
project(SymphonyOfEmpires CXX C)
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(CMAKE_CXX_STANDARD 20)

option(BUILD_ENGINE "Enable building Eng3D" ON)
option(BUILD_GAME "Enable building SymphonyOfEmpires" ON)

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

#
# Dependencies
#
set(SDL2_DISABLE_SDL2MAIN ON)
set(SDL2_DISABLE_INSTALL ON)
set(SDL2_DISABLE_UNINSTALL ON)
FetchContent_Declare(sdl2 GIT_REPOSITORY "https://github.com/libsdl-org/SDL" GIT_TAG main GIT_PROGRESS TRUE)
set(SDL2TTF_VENDORED ON)
set(SDL2TTF_INSTALL OFF)
set(SDL2TTF_SAMPLES OFF)
FetchContent_Declare(sdl2_ttf GIT_REPOSITORY "https://github.com/libsdl-org/SDL_ttf" GIT_TAG main GIT_PROGRESS TRUE)
set(LUA_ENABLE_SHARED ON)
set(LUA_BUILD_BINARY OFF)
set(LUA_BUILD_COMPILER OFF)
FetchContent_Declare(lua GIT_REPOSITORY "https://github.com/walterschell/Lua" GIT_PROGRESS TRUE)
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm" GIT_PROGRESS TRUE)
set(TBB_TEST OFF)
set(TBB_EXAMPLES OFF)
set(BUILD_SHARED_LIBS ON)
FetchContent_Declare(opentbb GIT_REPOSITORY "https://github.com/oneapi-src/oneTBB" GIT_PROGRESS TRUE)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_IGNORE_GIT_HASH ON)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ENABLE_BOOST_WORKAROUND ON)
FetchContent_Declare(assimp GIT_REPOSITORY "https://github.com/assimp/assimp" GIT_TAG c8dafe0d2887242285c0080c6cbbea8c1f1c8094 GIT_PROGRESS TRUE)
IF(NOT ANDROID)
	IF(E3D_BACKEND_OPENGL)
		set(GLFW_BUILD_TESTS OFF)
		set(GLFW_BUILD_EXAMPLES OFF)
		FetchContent_Declare(glfw GIT_REPOSITORY "https://github.com/glfw/glfw")
		set(EXTRA_FETCH_CONENT_LIST glfw)
	ENDIF()
ENDIF()
FetchContent_MakeAvailable(sdl2 sdl2_ttf lua glm opentbb assimp ${EXTRA_FETCH_CONENT_LIST})
include_directories("${lua_SOURCE_DIR}/lua-5.4.4/include" "${sdl2_BINARY_DIR}/include" "${sdl2_BINARY_DIR}/include-config-relwithdebinfo" "${sdl2_ttf_SOURCE_DIR}" "${sdl2_mixer_SOURCE_DIR}/include")

# Build stuff
IF(BUILD_ENGINE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/eng3d ${CMAKE_BINARY_DIR}/eng3d)
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
	link_directories("${DEVKITPRO}/libogc/lib")
	include_directories("${DEVKITPRO}/libogc/include")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_UNIX=1")
ENDIF()
IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
ENDIF()

include_directories("${PROJECT_SOURCE_DIR}/game/src" "${CMAKE_SOURCE_DIR}/eng3d/eng3d/include")

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

# Platform-specific libraries
target_link_libraries(SymphonyOfEmpires PUBLIC SDL2 SDL2_ttf SDL2_mixer lua glm tbb assimp)
IF(NOT ANDROID)
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(SymphonyOfEmpires PUBLIC glfw)
	ENDIF()
ELSEIF(ANDROID)
	target_link_libraries(SymphonyOfEmpires PUBLIC android)
ENDIF()

IF(WIN32)
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
		
	ELSE()
		target_link_libraries(SymphonyOfEmpires PUBLIC stdc++ m winpthread)
	ENDIF()
ELSE()
	target_link_libraries(SymphonyOfEmpires PUBLIC m stdc++)
ENDIF()
target_link_libraries(SymphonyOfEmpires PUBLIC eng3d)

# Pthread
find_package(Threads)
IF(Threads_FOUND)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	target_link_libraries(SymphonyOfEmpires PRIVATE Threads::Threads)
ENDIF()

set(APP_SHARED_LIBRARIES
	${LIBRARY_OUTPUT_PATH}/libtbb.so
	${LIBRARY_OUTPUT_PATH}/libassimp.so
	${LIBRARY_OUTPUT_PATH}/libSDL2.so
	${LIBRARY_OUTPUT_PATH}/libSDL2_ttf.so
	${LIBRARY_OUTPUT_PATH}/liblua.so
	${LIBRARY_OUTPUT_PATH}/libeng3d.so
)
