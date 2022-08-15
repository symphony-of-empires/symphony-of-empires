cmake_minimum_required(VERSION 3.15)
project(SymphonyOfEmpires CXX C)
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

option(BUILD_ENGINE "Enable building Eng3D" ON)
option(BUILD_GAME "Enable building SymphonyOfEmpires" ON)
option(SOE_UBSAN "Enable UBSAN instrumentation" OFF)

IF(SOE_UBSAN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
ENDIF()

IF(WIN32)
	add_definitions(-DNOMINMAX=1)
	add_definitions(-DWIN32_LEAN_AND_MEAN=1)
ELSEIF(ANDROID)
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

#
# Compilation flags
#
IF(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS}")
ENDIF()
IF(WIN32)
	add_definitions(-DE3D_TARGET_WINDOWS=1)
ELSEIF(UNIX)
	include_directories("/usr/include/SDL2")
	add_definitions(-DE3D_TARGET_UNIX=1)
	IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		# Workaround for clang (see https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023)
		link_directories(/usr/local/lib)
	ENDIF()
ELSEIF(ANDROID)
	add_definitions(-DE3D_TARGET_UNIX=1)
	add_definitions(-DE3D_TARGET_ANDROID=1)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
	include_directories("/usr/pkg/include/SDL2")
	add_definitions(-DE3D_TARGET_UNIX=1)
	add_definitions(-DE3D_TARGET_NETBSD=1)
	link_directories("${DEVKITPRO}/libogc/lib")
	include_directories("${DEVKITPRO}/libogc/include")
	add_definitions(-DE3D_TARGET_RVL=1)
ENDIF()
IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
ENDIF()

include_directories("${PROJECT_SOURCE_DIR}/game/src" "${CMAKE_SOURCE_DIR}/eng3d")

#
# Source code files
#

# C++ source files
file(GLOB_RECURSE MAIN_SOURCES
	"${PROJECT_SOURCE_DIR}/game/src/*.cpp"
)
IF(ANDROID)
	add_executable(SymphonyOfEmpires "${MAIN_SOURCES}" "${ANDROID_NDK_HOME}/sources/android/native_app_glue/android_native_app_glue.c")
ELSEIF(WIN32)
	# Build as a windows application
	add_executable(SymphonyOfEmpires WIN32 "${MAIN_SOURCES}")
ELSE()
	# The rest is a normal application
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

find_package(TBB)
IF(TBB_FOUND)
	target_link_libraries(SymphonyOfEmpires PRIVATE TBB::TBB)
	add_definitions(-DSOE_FEATURE_TBB)
ENDIF()

# Platform-specific libraries
IF(WIN32)
	target_link_libraries(SymphonyOfEmpires PRIVATE ws2_32 wsock32)
	IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR MSVC)
		target_link_libraries(eng3d PRIVATE libglew_static)
	ELSE()
		target_link_libraries(SymphonyOfEmpires PRIVATE m stdc++)
	ENDIF()
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(SymphonyOfEmpires PRIVATE opengl32)
	ENDIF()
ELSEIF(UNIX OR ANDROID)
	target_link_libraries(SymphonyOfEmpires PRIVATE m stdc++ atomic)
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(SymphonyOfEmpires PRIVATE GL GLEW)
	ELSEIF(E3D_BACKEND_GLES)
		target_link_libraries(SymphonyOfEmpires PRIVATE EGL GLESv1_CM GLESv2 GLESv3)
	ENDIF()
ENDIF()
target_link_libraries(SymphonyOfEmpires PUBLIC eng3d SDL2)

IF(ANDROID)
	set(APP_SHARED_LIBRARIES ${LIBRARY_OUTPUT_PATH}/libtbb.so ${LIBRARY_OUTPUT_PATH}/libSDL2.so ${LIBRARY_OUTPUT_PATH}/liblua.so ${LIBRARY_OUTPUT_PATH}/libeng3d.so)
	string(APPEND APP_SHARED_LIBRARIES ${LIBRARY_OUTPUT_PATH}/libassimp.so)
ENDIF()
)
