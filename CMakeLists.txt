cmake_minimum_required(VERSION 3.5.1)

project(SymphonyOfEmpires CXX C)

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

# Build stuff
IF(BUILD_ENGINE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/eng3d ${CMAKE_BINARY_DIR}/eng3d)
ENDIF()

#
# Compilation flags
#
set(CMAKE_CXX_STANDARD 20)

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

# Dependencies
set(ASSIMP_INSTALL ON)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_IGNORE_GIT_HASH ON)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/assimp)
include_directories(${CMAKE_SOURCE_DIR}/vendor/assimp/include)
include_directories(${CMAKE_BINARY_DIR}/vendor/assimp/include) # assimp generates config.h here
target_link_libraries(SymphonyOfEmpires PUBLIC assimp)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glm)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)
target_link_libraries(SymphonyOfEmpires PUBLIC glm)

set(TBB_TEST OFF)
set(TBB_EXAMPLES OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/onetbb)
include_directories(${CMAKE_SOURCE_DIR}/vendor/onetbb/include)
target_link_libraries(SymphonyOfEmpires PUBLIC tbb)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/sdl2)
include_directories(${CMAKE_SOURCE_DIR}/vendor/sdl2/include)
target_link_libraries(SymphonyOfEmpires PUBLIC SDL2 SDL2main)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/lua)
include_directories(${CMAKE_SOURCE_DIR}/vendor/lua)
target_link_libraries(SymphonyOfEmpires PUBLIC lua)

set(SDL2TTF_VENDORED ON)
set(SDL2TTF_SAMPLES OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/sdl2_ttf)
include_directories(${CMAKE_SOURCE_DIR}/vendor/sdl2_ttf)
target_link_libraries(SymphonyOfEmpires PUBLIC SDL2_ttf)

IF(ANDROID)
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
