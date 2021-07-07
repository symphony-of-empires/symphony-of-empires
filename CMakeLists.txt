cmake_minimum_required(VERSION 3.13)

project(SymphonyOfEmpires)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

# This is conflicting since we use cmake ..
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
#set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -std=c++17 -O2 -g -Isrc -Isrc/client -pthread")
if (WIN32)
# Use x86_64-w64-mingw-* toolchain for windows builds on linux
	set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc")
	set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Iwindows_libs/SDL2_include -Lwindows_libs/SDL2_libs")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Iwindows_libs/PNG_include -Lwindows_libs/PNG_libs")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Iwindows_libs/LUA_include -Lwindows_libs/LUA_libs")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Iwindows_libs/TBB_include -Lwindows_libs/TBB_libs")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Iwindows_libs/ZLIB_include -Lwindows_libs/ZLIB_libs")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Iwindows_libs/INTL_include -Lwindows_libs/INTL_libs")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Iwindows_libs/SDL2T_include -Lwindows_libs/SDL2T_libs")
ELSE()
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF()

include_directories("${PROJECT_SOURCE_DIR}")
link_directories(/usr/local/lib)

file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB CLIENT_SOURCES "${PROJECT_SOURCE_DIR}/client/*.cpp")

add_executable(SymphonyOfEmpires 
	${MAIN_SOURCES}
	${CLIENT_SOURCES}
)

if (WIN32)
	target_link_libraries(SymphonyOfEmpires PUBLIC opengl32 glu32 lua intl)
ELSE()
	target_link_libraries(SymphonyOfEmpires PUBLIC GL GLU lua5.4 SDL2main)
ENDIF()
target_link_libraries(SymphonyOfEmpires PUBLIC png SDL2 SDL2_ttf m tbb)
target_compile_features(SymphonyOfEmpires PRIVATE cxx_std_17)
