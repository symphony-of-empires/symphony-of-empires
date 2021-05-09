cmake_minimum_required(VERSION 3.13)

project(SymphonyOfEmpires)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -std=c++17 -O2 -fno-exceptions -fno-rtti -g -Isrc -Isrc/client -lSDL2main -lSDL2 -pthread")
if (WIN32)
	include_directories(${CMAKE_SOURCE_DIR}/dependencies_windows/SDL2_ttf-devel-2.0.15-VC/include/SDL2)
	include_directories(${CMAKE_SOURCE_DIR}/dependencies_windows/SDL2-devel-2.0.14-VC/include/SDL2)
	
	link_directories(${CMAKE_SOURCE_DIR}/dependencies_windows/SDL2_ttf-devel-2.0.15-VC/lib)
	link_directories(${CMAKE_SOURCE_DIR}/dependencies_windows/SDL2-devel-2.0.14-VC/lib)
ENDIF()

include_directories("${PROJECT_SOURCE_DIR}")
link_directories(/usr/local/lib)

file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB CLIENT_SOURCES "${PROJECT_SOURCE_DIR}/client/*.cpp")

add_executable(SymphonyOfEmpires 
	${MAIN_SOURCES}
	${CLIENT_SOURCES}
)

target_link_libraries(SymphonyOfEmpires LINK_PUBLIC opengl32 glu32 lua intl png SDL2 SDL2_ttf)