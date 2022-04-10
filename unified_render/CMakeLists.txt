cmake_minimum_required(VERSION 3.5.1)

project(unified_render)

option(UR_BUILD_ENGINE "Enable building the UnifiedRender library" ON)

#
# Defines - see README.1st
#
option(UR_RENDER_DEBUG, "UR_RENDER_DEBUG" ON)
IF(UR_RENDER_DEBUG)
	add_definitions(-DUR_RENDER_DEBUG=1)
ENDIF()

option(UR_HEAP_DEBUG, "UR_HEAP_DEBUG" ON)
IF(UR_HEAP_DEBUG)
	add_definitions(-DUR_HEAP_DEBUG=1)
ENDIF()

option(UR_BACKEND_OPENGL, "UR_BACKEND_OPENGL" ON)
IF(UR_HEAP_DEBUG)
	add_definitions(-DUR_BACKEND_OPENGL=1)
ENDIF()

option(UR_MANAGED_HEAP, "UR_MANAGED_HEAP" ON)
IF(UR_MANAGED_HEAP)
	add_definitions(-DUR_MANAGED_HEAP=1)
ENDIF()

option(UR_RTTI, "UR_RTTI" ON)
IF(UR_RTTI)
	add_definitions(-DUR_RTTI=1)
	set(CMAKE_CXX_FLAGS "-fno-rtti")
ENDIF()

option(UR_EXCEPTIONS, "UR_EXCEPTIONS" ON)
IF(UR_EXCEPTIONS)
	add_definitions(-DUR_EXCEPTIONS=1)
	set(CMAKE_CXX_FLAGS "-fno-exceptions")
ENDIF()

#
# Compilation flags
#
set(CMAKE_CXX_STANDARD 20)
IF(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS} -pthread")
ENDIF()
IF(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dwindows=1")
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Dunix=1")
ENDIF()

IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST=1")
ENDIF()

add_subdirectory(${PROJECT_SOURCE_DIR}/unified_render)
