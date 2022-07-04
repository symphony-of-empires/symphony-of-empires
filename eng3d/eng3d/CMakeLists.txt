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
set(SDL2MIXER_INSTALL OFF)
set(SDL2MIXER_SAMPLES OFF)
set(SDL2MIXER_VENDORED ON)
set(SDL2MIXER_FLAC OFF)
set(SDL2MIXER_MOD OFF)
set(SDL2MIXER_MP3 OFF)
set(SDL2MIXER_MIDI OFF)
set(SDL2MIXER_OPUS OFF)
set(SDL2MIXER_WAVE OFF)
set(SDL2MIXER_CMD OFF)
FetchContent_Declare(sdl2_mixer GIT_REPOSITORY "https://github.com/libsdl-org/SDL_mixer" GIT_TAG main GIT_PROGRESS TRUE)
FetchContent_Declare(lua GIT_REPOSITORY "https://github.com/andyroiiid/lua" GIT_TAG 031b4661b22cbd7fb781d53d20460e9cc59ec2c1 GIT_PROGRESS TRUE)
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm" GIT_PROGRESS TRUE)
set(TBB_TEST OFF)
set(TBB_EXAMPLES OFF)
FetchContent_Declare(opentbb GIT_REPOSITORY "https://github.com/oneapi-src/oneTBB" GIT_PROGRESS TRUE)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_IGNORE_GIT_HASH ON)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_TESTS OFF)
FetchContent_Declare(assimp GIT_REPOSITORY "https://github.com/assimp/assimp" GIT_TAG c8dafe0d2887242285c0080c6cbbea8c1f1c8094 GIT_PROGRESS TRUE)
IF(NOT ANDROID)
	IF(E3D_BACKEND_OPENGL)
		set(GLFW_BUILD_TESTS OFF)
		set(GLFW_BUILD_EXAMPLES OFF)
		FetchContent_Declare(glfw GIT_REPOSITORY "https://github.com/glfw/glfw")
		set(EXTRA_FETCH_CONENT_LIST glfw)
	ENDIF()
ENDIF()
FetchContent_MakeAvailable(sdl2 sdl2_ttf sdl2_mixer lua glm opentbb assimp ${EXTRA_FETCH_CONENT_LIST})
include_directories("${sdl2_BINARY_DIR}/include" "${sdl2_BINARY_DIR}/include-config-relwithdebinfo" "${sdl2_ttf_SOURCE_DIR}" "${sdl2_mixer_SOURCE_DIR}/include")

#
# Source code files
#
file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/ui/*.cpp")
add_library(eng3d STATIC ${MAIN_SOURCES})

#
# Linking
#
target_link_libraries(eng3d PUBLIC SDL2 SDL2_ttf SDL2_mixer lua glm tbb assimp)
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
