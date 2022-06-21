cmake_minimum_required(VERSION 3.5.1)

project(eng3d CXX C)

option(E3D_BUILD_ENGINE "Enable building the Eng3D library" ON)

#
# Defines - see README.1st
#
option(E3D_DEBUG, "E3D_DEBUG" ON)
IF(E3D_DEBUG)
	add_definitions(-DE3D_DEBUG=1)
ENDIF()

option(E3D_BACKEND_OPENGL, "Enable the OpenGL backend" ON)
IF(E3D_BACKEND_OPENGL)
	add_definitions(-DE3D_BACKEND_OPENGL=1)
ENDIF()

option(E3D_BACKEND_GLES, "Enable the GLES backend" ON)
IF(E3D_BACKEND_GLES)
	add_definitions(-DE3D_BACKEND_GLES=1)
ENDIF()

option(E3D_BACKEND_RGX, "E3D_BACKEND_RGX" ON)
IF(E3D_BACKEND_RGX)
	add_definitions(-DE3D_BACKEND_RGX=1)
ENDIF()

option(E3D_MANAGED_HEAP, "Allow Eng3D to manage heap (useful for debugging without instrumentation)" ON)
IF(E3D_MANAGED_HEAP)
	add_definitions(-DE3D_MANAGED_HEAP=1)
ENDIF()

#
# Compilation flags
#
set(CMAKE_CXX_STANDARD 20)
IF(NOT MSVC)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra ${CMAKE_CXX_FLAGS}")
ENDIF()
IF(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_WINDOWS=1")
ELSEIF(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_UNIX=1")
ELSEIF(ANDROID)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_UNIX=1 -DE3D_TARGET_ANDROID=1")
ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DE3D_TARGET_RVL=1")
ENDIF()

IF(UNIT_TEST)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUNIT_TEST=1")
ENDIF()

add_subdirectory(${PROJECT_SOURCE_DIR}/eng3d)
