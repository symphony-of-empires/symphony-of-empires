cmake_minimum_required(VERSION 3.13)

project(SymphonyOfEmpires)

#set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/src)
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Release-Linux)
if (WIN32)

ELSE()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wshadow -O2 -fno-exceptions -fno-rtti -g -Isrc -Isrc/client -lSDL2main -lSDL2 -pthread")
ENDIF()

include_directories("${PROJECT_SOURCE_DIR}")

# For Libraries

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# For Sources
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
target_link_libraries(SymphonyOfEmpires PUBLIC ${CONAN_LIBS} png SDL2 SDL2_ttf m)

target_compile_features(SymphonyOfEmpires PRIVATE cxx_std_17)
