cmake_minimum_required(VERSION 3.5.1)

project(unified_render)
option(UR_BUILD_ENGINE "Enable building the UnifiedRender library" ON)

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
