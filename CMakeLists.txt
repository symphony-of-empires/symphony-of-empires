cmake_minimum_required(VERSION 3.20)

set(DEVKITPRO $ENV{DEVKITPRO})
IF(DEVKITPRO AND FALSE)
	message(STATUS "Using DevKitPro")
	list(APPEND CMAKE_MODULE_PATH "$ENV{DEVKITPRO}/cmake")
	include(Switch)
	include(Platform/NintendoSwitch)
ENDIF()

set(FETCHCONTENT_QUIET FALSE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

project(SymphonyOfEmpires CXX C)
include(FetchContent)

option(BUILD_ENGINE "Enable building Eng3D" ON)
option(BUILD_GAME "Enable building SymphonyOfEmpires" ON)
option(SOE_UBSAN "Enable UBSAN instrumentation" OFF)

IF(SOE_UBSAN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined,thread")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined,thread")
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
IF(E3D_BACKEND_GLES)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_BACKEND_GLES=1")
ELSEIF(E3D_BACKEND_D3D9)
	IF(NOT WIN32)
		message(FATAL_ERROR "DirectX 9 is not supported in ${CMAKE_SYSTEM_NAME} platform")
	ENDIF()
	message("Using DirectX 9 SDK")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_BACKEND_D3D9=1")
ELSE()
	IF(NOT E3D_BACKEND_OPENGL)
		message(WARNING "No backend specified - using OpenGL as default")
		set(E3D_BACKEND_OPENGL ON)
	ENDIF()
	IF(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch" OR ANDROID)
		message(FATAL_ERROR "OpenGL is not supported in ${CMAKE_SYSTEM_NAME} platform, try GLES instead")
	ENDIF()
	message("Using OpenGL backend")
	IF(ANDROID)
		message(FATAL_ERROR "Use E3D_BACKEND_GLES instead on Android")
	ENDIF()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_BACKEND_OPENGL=1")
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
IF(UNIT_TEST)
	add_definitions(-DUNIT_TEST=1)
ENDIF()
IF(WIN32)
	add_definitions(-DE3D_TARGET_WINDOWS=1)
ELSEIF(UNIX)
	add_definitions(-DE3D_TARGET_UNIX=1)
	IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		# Workaround for clang (see https://bugs.launchpad.net/ubuntu/+source/libsdl2-ttf/+bug/1872023)
		link_directories(/usr/local/lib)
	ENDIF()
ELSEIF(ANDROID)
	add_definitions(-DE3D_TARGET_UNIX=1 -DE3D_TARGET_ANDROID=1)
	add_definitions(-DE3D_HANDHELD=1)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
	add_definitions(-DE3D_TARGET_UNIX=1)
	add_definitions(-DE3D_TARGET_NETBSD=1)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
	include_directories("${DEVKITPRO}/portlibs/switch/include/")
	add_definitions(-DE3D_TARGET_UNIX=1 -DE3D_TARGET_SWITCH=1)
	add_definitions(-DE3D_HANDHELD=1)
	# Compiler is for 32bits and doesn't support long long
	add_definitions(-DLUA_32BITS=1)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NintendoWii")
	link_directories("${DEVKITPRO}/libogc/lib")
	include_directories("${DEVKITPRO}/libogc/include")
	add_definitions(-DE3D_TARGET_RVL=1)
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
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
	target_include_directories(SymphonyOfEmpires PRIVATE "/opt/devkitpro/portlibs/switch/include/")
ELSE()
	# The rest is a normal application
	add_executable(SymphonyOfEmpires "${MAIN_SOURCES}")
ENDIF()

# LUA
set(LUA_BUILD_BINARY OFF)
set(LUA_BUILD_COMPILER OFF)
IF(E3D_BUILD_STATIC OR CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
	set(LUA_ENABLE_SHARED OFF)
	set(LUA_SUPPORT_DL OFF)
ELSE()
	set(LUA_ENABLE_SHARED ON)
	set(LUA_SUPPORT_DL ON)
ENDIF()
FetchContent_Declare(lua GIT_REPOSITORY "https://github.com/walterschell/Lua" GIT_PROGRESS TRUE)
FetchContent_MakeAvailable(lua)
target_include_directories(SymphonyOfEmpires PRIVATE "${lua_SOURCE_DIR}/lua-5.4.4/include")
target_link_libraries(SymphonyOfEmpires PUBLIC lua_static)

#
# Linking
#

# Pthread
find_package(Threads)
IF(Threads_FOUND)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	target_link_libraries(SymphonyOfEmpires PRIVATE Threads::Threads)
ENDIF()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
IF(NOT E3D_VENDORED_TBB)
	find_package(TBB REQUIRED)
	target_link_libraries(SymphonyOfEmpires PRIVATE TBB::tbb)
ELSE()
	message(STATUS "Using vendored TBB")
	# TBB (and hwloc if needed)
	IF(WIN32)
		FetchContent_Declare(hwloc URL "https://download.open-mpi.org/release/hwloc/v2.7/hwloc-win64-build-2.7.1.zip")
		FetchContent_MakeAvailable(hwloc)
		set(CMAKE_HWLOC_2_5_DLL_PATH "${hwloc_SOURCE_DIR}/lib/libhwloc.dll")
		set(CMAKE_HWLOC_2_5_LIBRARY_PATH "${hwloc_SOURCE_DIR}/lib/libhwloc.lib" )
		set(CMAKE_HWLOC_2_5_INCLUDE_PATH "${hwloc_SOURCE_DIR}/include" )
	ENDIF()
	set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH ON)
	set(TBB_TEST OFF)
	set(TBB_EXAMPLES OFF)
	set(TBB_ENABLE_IPO OFF)
	set(TBB_CPF OFF)
	set(TBBMALLOC_BUILD ON)
	set(TBBMALLOC_PROXY_BUILD ON)
	set(TBB4PY_BUILD OFF)
	set(TBB_SIGNTOOL OFF)
	IF(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
		set(BUILD_SHARED_LIBS OFF)
	ENDIF()
	FetchContent_Declare(tbb GIT_REPOSITORY "https://github.com/oneapi-src/oneTBB" GIT_TAG dce182a7b554becada120472659be1b25f89051c GIT_PROGRESS TRUE)
	FetchContent_MakeAvailable(tbb)
	target_include_directories(SymphonyOfEmpires PRIVATE "${tbb_SOURCE_DIR}/include")
	target_link_libraries(SymphonyOfEmpires PRIVATE tbb)
ENDIF()

IF(WIN32)
	target_link_directories(SymphonyOfEmpires PRIVATE "D:/a/symphony-of-empires/symphony-of-empires/builds/msbuild-vcpkg/")
	target_link_libraries(SymphonyOfEmpires PRIVATE "D:/a/symphony-of-empires/symphony-of-empires/builds/msbuild-vcpkg/eng3d.dll")
ELSE()
	target_link_libraries(SymphonyOfEmpires PRIVATE eng3d)
ENDIF()

IF(ANDROID)
	set(APP_SHARED_LIBRARIES ${LIBRARY_OUTPUT_PATH}/libtbb.so ${LIBRARY_OUTPUT_PATH}/libSDL2.so ${LIBRARY_OUTPUT_PATH}/liblua.so ${LIBRARY_OUTPUT_PATH}/libeng3d.so)
	string(APPEND APP_SHARED_LIBRARIES ${LIBRARY_OUTPUT_PATH}/libassimp.so)
ENDIF()

# Build stuff
IF(BUILD_ENGINE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/eng3d ${CMAKE_BINARY_DIR}/eng3d)
	add_dependencies(SymphonyOfEmpires eng3d)
ENDIF()

#IF(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
	message(STATUS "Creating NRO file")
	set(NROFLAGS "")
	string(APPEND NROFLAGS " --romfsdir=/home/wxwisiasdf/src/symphony-of-empires/mods")
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/SymphonyOfEmpires.nro
		COMMAND ${NX_ELF2NRO_EXE} $<TARGET_FILE:SymphonyOfEmpires> ${CMAKE_CURRENT_BINARY_DIR}/SymphonyOfEmpires.nro ${NROFLAGS}
		DEPENDS SymphonyOfEmpires
		VERBATIM
	)
#ENDIF()
