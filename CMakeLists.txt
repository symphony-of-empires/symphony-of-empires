cmake_minimum_required(VERSION 3.16.3)

project(SymphonyOfEmpires)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -std=c++17 -O2 -g -Isrc -Isrc/client -pthread")
IF(WIN32)
# Use x86_64-w64-mingw-* toolchain for windows builds on linux
	set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc-posix")
	set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++-posix")
	
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-expansion-to-defined")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dwindows -mwindows")
	
	link_directories(${CMAKE_SOURCE_DIR}/windows_libs/lib)
	include_directories(${CMAKE_SOURCE_DIR}/windows_libs/include)
	include_directories(${CMAKE_SOURCE_DIR}/windows_libs/include/SDL2)
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dunix")
ENDIF()

# Helps github workflows to not die
IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST")
ENDIF()

include_directories("${PROJECT_SOURCE_DIR}")
link_directories(/usr/local/lib)

file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB CLIENT_SOURCES "${PROJECT_SOURCE_DIR}/client/*.cpp")

add_executable(SymphonyOfEmpires ${MAIN_SOURCES} ${CLIENT_SOURCES})

IF(WIN32)
	target_link_libraries(SymphonyOfEmpires PUBLIC opengl32 glu32 ws2_32 winpthread intl lua)
ELSE()
	target_link_libraries(SymphonyOfEmpires PUBLIC GL GLU SDL2main lua5.4)
ENDIF()
target_link_libraries(SymphonyOfEmpires PUBLIC tbb png SDL2 SDL2_ttf m z)
