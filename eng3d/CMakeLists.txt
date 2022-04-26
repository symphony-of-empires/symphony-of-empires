cmake_minimum_required(VERSION 3.5.1)

project(eng3d)

option(E3D_BUILD_ENGINE "Enable building the Eng3D library" ON)

#
# Defines - see README.1st
#
option(E3D_RENDER_DEBUG, "E3D_RENDER_DEBUG" ON)
IF(E3D_RENDER_DEBUG)
	add_definitions(-DE3D_RENDER_DEBUG=1)
ENDIF()

option(E3D_HEAP_DEBUG, "E3D_HEAP_DEBUG" ON)
IF(E3D_HEAP_DEBUG)
	add_definitions(-DE3D_HEAP_DEBUG=1)
ENDIF()

option(E3D_BACKEND_OPENGL, "E3D_BACKEND_OPENGL" ON)
IF(E3D_HEAP_DEBUG)
	add_definitions(-DE3D_BACKEND_OPENGL=1)
ENDIF()

option(E3D_MANAGED_HEAP, "E3D_MANAGED_HEAP" ON)
IF(E3D_MANAGED_HEAP)
	add_definitions(-DE3D_MANAGED_HEAP=1)
ENDIF()

option(E3D_RTTI, "E3D_RTTI" ON)
IF(E3D_RTTI)
	add_definitions(-DE3D_RTTI=1)
	set(CMAKE_CXX_FLAGS "-fno-rtti")
ENDIF()

option(E3D_EXCEPTIONS, "E3D_EXCEPTIONS" ON)
IF(E3D_EXCEPTIONS)
	add_definitions(-DE3D_EXCEPTIONS=1)
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

add_subdirectory(${PROJECT_SOURCE_DIR}/eng3d)
