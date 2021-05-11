cmake_minimum_required(VERSION 3.18)

project(SymphonyOfEmpires)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -std=c++17 -O2 -fno-exceptions -fno-rtti -g -lSDL2main -lSDL2 -pthread")
if (WIN32)
	include_directories("C:/msys64/mingw64/include")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWINDOWS")
	add_compile_definitions("_WIN32")
ENDIF(WIN32)

# PNG
INCLUDE(FindPNG)
IF(PNG_FOUND)
	MESSAGE(STATUS "Looking for PNG - found")
	include_directories(${PNG_INCLUDE_DIRS})
	link_directories(${PNG_LIBRARIES})
ELSE(PNG_FOUND)
	MESSAGE(STATUS "Looking for PNG - not found")
ENDIF(PNG_FOUND)

# LUA
INCLUDE(FindLua)
IF(LUA_FOUND)
	MESSAGE(STATUS "Looking for Lua - found")
	include_directories(${LUA_INCLUDE_DIR})
	link_directories(${LUA_LIBRARIES})
ELSE(LUA_FOUND)
	MESSAGE(STATUS "Looking for Lua - not found")
ENDIF(LUA_FOUND)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/client")
link_directories(/usr/local/lib)

file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB CLIENT_SOURCES "${PROJECT_SOURCE_DIR}/client/*.cpp")

add_executable(SymphonyOfEmpires 
	${MAIN_SOURCES}
	${CLIENT_SOURCES}
)
if (WIN32)
	target_link_libraries(SymphonyOfEmpires PUBLIC opengl32 glu32 lua intl)
ELSE(WIN32)
	target_link_libraries(SymphonyOfEmpires PUBLIC GL GLU lua5.4)
ENDIF(WIN32)
target_link_libraries(SymphonyOfEmpires PUBLIC png SDL2 SDL2_ttf m)