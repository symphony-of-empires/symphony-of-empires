cmake_minimum_required(VERSION 3.15)
project(eng3d CXX C)

#
# Defines - see README.1st
#
option(E3D_BUILD_ENGINE "Enable building the Eng3D library" ON)
option(E3D_DEBUG, "Enable debug" ON)
option(E3D_BACKEND_OPENGL, "Enable the OpenGL backend" OFF)
option(E3D_BACKEND_GLES, "Enable the GLES backend" ON)
option(E3D_BACKEND_RGX, "Enable dummy graphics" OFF)
option(E3D_MANAGED_HEAP, "Allow Eng3D to manage heap (useful for debugging without instrumentation)" ON)
option(E3D_PIE, "Build as a position-independent library" ON)
option(E3D_BUILD_STATIC, "Whetever to make a static library or not" OFF)
option(E3D_ENABLE_IPO "Enables IPO if supported" OFF)
option(E3D_ZLIB_VENDORED "Whetever to build zlib or not" ON)
option(E3D_TBB_VENDORED "Whetever to build tbb or not" ON)
option(E3D_SDL2_VENDORED "Whetever to build SDL2 or not" ON)

IF(E3D_DEBUG)
	add_definitions(-DE3D_DEBUG=1)
ENDIF()

IF(E3D_BACKEND_OPENGL)
	add_definitions(-DE3D_BACKEND_OPENGL=1)
ELSEIF(E3D_BACKEND_D3D9)
	add_definitions(-DE3D_BACKEND_D3D9=1)
ELSEIF(E3D_BACKEND_GLES)
	add_definitions(-DE3D_BACKEND_GLES=1)
ELSEIF(E3D_BACKEND_RGX)
	add_definitions(-DE3D_BACKEND_RGX=1)
ENDIF()

IF(E3D_MANAGED_HEAP)
	add_definitions(-DE3D_MANAGED_HEAP=1)
ENDIF()

#
# Compilation flags
#
IF(NOT MSVC)
	add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
ENDIF()
IF(UNIT_TEST)
	add_definitions(-DUNIT_TEST=1)
ENDIF()

IF(WIN32)
	# ...
ELSEIF(UNIX)
	# ...
ELSEIF(ANDROID)
	add_definitions(-DE3D_HANDHELD=1)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
	include_directories("/usr/pkg/include/SDL2")
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
	include_directories("${DEVKITPRO}/portlibs/switch/include/")
	add_definitions(-DE3D_HANDHELD=1)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NintendoWii")
	link_directories("${DEVKITPRO}/libogc/lib")
	include_directories("${DEVKITPRO}/libogc/include")
ENDIF()

#
# Dependencies
#
IF(NOT E3D_PIE AND CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
	message(WARNING "Enabling PIE")
	set(E3D_PIE ON)
ENDIF()
set(CMAKE_POSITION_INDEPENDENT_CODE "${E3D_PIE}")

#
# Source code files
#
file(GLOB ENG3D_SOURCES "${PROJECT_SOURCE_DIR}/eng3d/*.c" "${PROJECT_SOURCE_DIR}/eng3d/*.cpp" "${PROJECT_SOURCE_DIR}/eng3d/ui/*.cpp")
# Add android native C glue
#IF(ANDROID)
#	list(APPEND ENG3D_SOURCES "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
#ENDIF()
IF(E3D_BUILD_STATIC)
	message(STATUS "Building a static library")
	add_library(eng3d STATIC ${ENG3D_SOURCES})
ELSE()
	message(STATUS "Building a shared library")
	add_library(eng3d SHARED ${ENG3D_SOURCES})
ENDIF()
target_include_directories(eng3d PRIVATE "${PROJECT_SOURCE_DIR}")
set_property(TARGET eng3d PROPERTY CMAKE_CXX_STANDARD 20)
set_property(TARGET eng3d PROPERTY CMAKE_FIND_LIBRARY_SUFFIXES ".a")

# Enable LTO/IPO if supported by the compiler
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)
IF(IPO_SUPPORTED AND E3D_ENABLE_IPO)
	message(STATUS "Enabling IPO")
	set_property(TARGET eng3d PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
ELSE()
	message(STATUS "IPO not supported ${IPO_ERROR}")
ENDIF()

#
# Linking
#
IF(WIN32)
	target_link_libraries(eng3d PRIVATE ws2_32 wsock32)
	IF(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR MSVC))
		target_link_libraries(eng3d PUBLIC m stdc++)
	ENDIF()
	IF(E3D_BACKEND_OPENGL)
		target_link_libraries(eng3d PUBLIC opengl32)
	ENDIF()
ELSEIF(UNIX OR ANDROID)
	target_link_libraries(eng3d PUBLIC m stdc++ atomic)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
	target_link_libraries(eng3d PUBLIC stdc++)
	target_link_libraries(eng3d PUBLIC drm_nouveau EGL GLESv2 glad glapi)
	target_link_libraries(eng3d PUBLIC freetype bz2) # SDL2_ttf
	target_link_libraries(eng3d PUBLIC SDL2main SDL2_test png16 minizip)
	target_include_directories(eng3d PUBLIC "${NX_ROOT}/include")
	target_link_directories(eng3d PUBLIC "${NX_ROOT}/lib")
ENDIF()

# pthread
find_package(Threads)
IF(Threads_FOUND)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	target_link_libraries(eng3d PUBLIC Threads::Threads)
ENDIF()

IF((WIN32 OR UNIX) AND (NOT ANDROID))
	target_link_libraries(eng3d PUBLIC dependency_GLEW)
ENDIF()

add_dependencies(eng3d dependency_zlib)

target_link_libraries(eng3d PUBLIC
	dependency_OpenGL
	dependency_SDL2
	dependency_SDL2_ttf
	dependency_SDL2_mixer
	dependency_tbb
	#dependency_assimp
	dependency_glm
	dependency_lua
	dependency_zlib
	dependency_eventpp
	dependency_freetype
)

IF(WIN32)
    add_custom_command(TARGET eng3d
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2_ttf>
            $<TARGET_FILE:SDL2_mixer>
            $<TARGET_FILE:SDL2>
            ${CMAKE_BINARY_DIR})
ENDIF()

#file(GLOB ENG3D_INCLUDES "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/ui/*.hpp")
#install(TARGETS eng3d DESTINATION lib)
#install(FILES "${ENG3D_INCLUDES}" DESTINATION include)

add_executable(triangle ${PROJECT_SOURCE_DIR}/tests/triangle.cpp)
target_link_libraries(triangle PUBLIC eng3d)

add_executable(archive ${PROJECT_SOURCE_DIR}/tests/archive.cpp)
target_link_libraries(archive PUBLIC eng3d)
