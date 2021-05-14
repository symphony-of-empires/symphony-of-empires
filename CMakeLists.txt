cmake_minimum_required(VERSION 3.19)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(SymphonyOfEmpires)

set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

foreach(_config ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${_config} _config)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${_config} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${_config} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${_config} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endforeach()

include_directories("${PROJECT_SOURCE_DIR}")

file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB CLIENT_SOURCES "${PROJECT_SOURCE_DIR}/client/*.cpp")

add_executable(SymphonyOfEmpires 
	${MAIN_SOURCES}
	${CLIENT_SOURCES}
)

find_package(Lua REQUIRED)
find_package(OpenGL REQUIRED)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
find_package(libpng CONFIG REQUIRED)

target_link_libraries(SymphonyOfEmpires PRIVATE png)
target_link_libraries(SymphonyOfEmpires PRIVATE ${LUA_LIBRARY})
target_link_libraries(SymphonyOfEmpires PUBLIC OpenGL::GL OpenGL::GLU)
target_link_libraries(SymphonyOfEmpires PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(SymphonyOfEmpires PRIVATE SDL2::SDL2_ttf)