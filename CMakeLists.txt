cmake_minimum_required(VERSION 3.13)

project(SymphonyOfEmpires)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
if (WIN32)
	include_directories("${CMAKE_SOURCE_DIR}/windows_dependencies/SDL2_ttf-devel-2.0.15-VC/include/SDL2")
	include_directories("${CMAKE_SOURCE_DIR}/windows_dependencies/SDL2-devel-2.0.14-VC/include/SDL2")
	include_directories("C:/Program Files/zlib/include")

	link_directories("${CMAKE_SOURCE_DIR}/windows_dependencies/SDL2_ttf-devel-2.0.15-VC/lib")
	link_directories("${CMAKE_SOURCE_DIR}/windows_dependencies/SDL2-devel-2.0.14-VC/lib")
	link_directories("C:/Program Files/zlib/lib")

ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -std=c++17 -O2 -fno-exceptions -fno-rtti -g -Isrc -Isrc/client -lSDL2main -lSDL2 -pthread")
ENDIF()

# PNG
INCLUDE(FindPNG)
IF(PNG_FOUND)
	MESSAGE(STATUS "Looking for PNG - found")
	include_directories(${PNG_INCLUDE_DIRS})
	link_directories(${PNG_LIBRARIES})
ELSE(PNG_FOUND)
	MESSAGE(STATUS "Looking for PNG - not found")
ENDIF(PNG_FOUND)

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
	target_link_libraries(SymphonyOfEmpires PUBLIC GL GLU lua5.4)
ENDIF()
target_link_libraries(SymphonyOfEmpires PUBLIC png SDL2 SDL2_ttf m)