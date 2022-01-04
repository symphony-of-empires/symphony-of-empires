cmake_minimum_required(VERSION 3.5.1)

project(SymphonyOfEmpires)

option(BUILD_ENGINE "Enable building UnifiedRender" ON)
option(BUILD_GAME "Enable building SymphonyOfEmpires" ON)

if(BUILD_ENGINE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/unified_render)
endif()

if(BUILD_GAME)
    add_subdirectory(${CMAKE_SOURCE_DIR}/game)
endif()